/******************************************************************************
 *
 *                    FAT16 FILESYSTEM IMPLEMENTATION
 *
 ******************************************************************************
 * FileName:        FAT.c
 * Dependencies:    See include below
 * Processor:       PIC24/Daytona
 * Compiler:        MPLAB C30 Compiler
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 * Microchip Technology Incorporated ("Microchip) and its licensors retain 
 * all ownership and intellectual property rights in the code accompanying 
 * this message and in all derivatives thereto.  The code and information 
 * accompanying this message is provided for your convenience and use with 
 * Microchip products only.  Your acceptance and/or use of this code 
 * constitute agreement to the terms and conditions of this notice.Microchip 
 * disclaims all liability arising from this information and its use.  It is 
 * your responsibility to ensure that your application meets with your 
 * specifications.Further, the implementation and use of the FAT file system 
 * accompanying this code, SD card specifications, MMC card specifications 
 * and other third party tools may require a license from various entities, 
 * including, but not limited to Microsoft® Corporation, SD Card Association 
 * and MMCA.  It is your responsibility to obtain more information regarding 
 * any applicable licensing obligations.
 * 
 * CODE AND INFORMATION ACCOMPANYING THIS MESSAGE IS PROVIDED “AS IS.”  
 * MICROCHIP AND ITS LICENSORS MAKES NO REPRESENTATION OR WARRANTIES OF ANY 
 * KIND WHETHER EXPRESS OR IMPLIED, WRITTEN OR ORAL, STATUTORY OR OTHERWISE, 
 * RELATED TO THE INFORMATION PROVIDED TO YOU, INCLUDING BUT NOT LIMITED TO 
 * ITS CONDITION, QUALITY, PERFORMANCE, MERCHANTABILITY, NON-INFRINGEMENT, 
 * OR FITNESS FOR PURPOSE.  MICROCHIP AND ITS LICENSORS ARE NOT LIABLE, UNDER 
 * ANY CIRCUMSTANCES, FOR SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, 
 * FOR ANY REASON WHATSOEVER.  	 
 *
 * ****************************************************************************
 * 
 * $Id: FAT.c,v 1.21 2006/10/24 16:37:51 C12923 Exp $
 *
 * $Name:  $
 *
 *****************************************************************************/
#include <string.h>
#include <fat16\fat.h>
#include <fat16\media.h>

/******************************************************************************
 * Function:        CETYPE LoadMBR( DISK *dsk )
 *
 * PreCondition:    Called from DiskMount()
 *
 * Input:           dsk - Pointer to disk containing the master boot
 *                        record to be loaded
 *
 * Output:          CE_GOOD - MBR loaded successfully
 *                  CE_BAD_SECTOR_READ - A bad sector read occurred
 *                  CE_BAD_PARTITION - The boot record is bad
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to load the MBR and extracts the
 *                  necessary information
 *
 * Note:            Should not be called by user
 *****************************************************************************/
static CETYPE LoadMBR( UINT idx, DISK *dsk )
{
    PT_FAT PartitionTable;
    BYTE error = CE_GOOD;
    BYTE type;

    // Get the partition table from the MBR
    if( !MediaReadSectorIdx( idx, FO_MBR, dsk->buffer ) )
       error = CE_BAD_SECTOR_READ;

    else {
       // Assign it the partition table structure
       PartitionTable = (PT_FAT)dsk->buffer;

       // Validate the MBR
       if( (PartitionTable->Signature0 != FAT_GOOD_SIGN_0) ||
           (PartitionTable->Signature1 != FAT_GOOD_SIGN_1) ) {
          error = CE_BAD_PARTITION;

       } else {   // Valid MBR loaded

          // Get the 32-bit offset to the first partition
          dsk->firsts = PartitionTable->Partition0.PTE_FrstSect;

          // Check if the partition type is acceptable
          type = PartitionTable->Partition0.PTE_FSDesc;

          switch( type ) {
             case PT_FAT12:
                 dsk->type = FAT12;
                 error = CE_CARDFAT12;   // FAT12 not supported
                 break;

             case PT_FAT16:
             case PT_FAT16_32MB:
             case PT_FAT16_LBA:
                 dsk->type = FAT16;
                 break;

             case PT_FAT32_LBA:
             case PT_FAT32:
                 dsk->type = FAT32;
                 error = CE_CARDFAT32;   // FAT32 not supported
                 break;

             default:
                 dsk->type = FAT16;
                 dsk->firsts = 0;
                 //error = CE_BAD_PARTITION;
                 break;
          }   // switch

       }   // Valid MBR
    }

    return( error );

}   // LoadMBR()


/******************************************************************************
 * Function:        static CETYPE LoadBootSector( DISK *dsk )
 *
 * PreCondition:    Called from DiskMount()
 *
 * Input:           dsk - Pointer to disk containing the boot sector
 *
 * Output:          CE_GOOD - Boot sector loaded
 *                  CE_BAD_SECTOR_READ - A bad sector read occurred
 *                  CE_NOT_FORMATTED - The disk is of an unsupported format
 *                  CE_CARDFAT12 - FAT12 card not supported
 *                  CE_CARDFAT32 - FAT32 card not supported
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to load the boot sector information
 *                  and extract the necessary information
 *
 * Note:            Should not be called by user
 *****************************************************************************/
static CETYPE LoadBootSector( UINT idx, DISK *dsk )
{
    WORD RootDirSectors;
    DWORD TotSec, DataSec;
    BYTE error = CE_GOOD;
    BS BootSec;   // Boot sector, assume it's FAT16
    WORD BytesPerSec;

    // Get the Boot sector
    if( !MediaReadSectorIdx(idx,  dsk->firsts, dsk->buffer ) )
       error = CE_BAD_SECTOR_READ;

    else {
        // Assign the type
       BootSec = (BS)dsk->buffer;

       // Verify the Boot Sector is valid
       if( (BootSec->Signature0 != FAT_GOOD_SIGN_0) ||
           (BootSec->Signature1 != FAT_GOOD_SIGN_1) ) {
          error = CE_NOT_FORMATTED;

       } else 
       {
            BPB_FAT16 bpb_fat16;

            bpb_fat16 = (BPB_FAT16)dsk->buffer;

            // Determine the number of sectors in one FAT (FATSz)
            dsk->fatsize = bpb_fat16->BPB_FATSz16;
                 


            // Figure out the total number of sectors
            TotSec = (DWORD)bpb_fat16->BPB_TotSec16;
            if( !TotSec )
             TotSec = bpb_fat16->BPB_TotSec32;

            if( !TotSec )   // Cannot be 0 or odd
             return( CE_NOT_FORMATTED );

            // 6. Get the full partition/drive layout
            // 6.1 Determine the size of a cluster
            dsk->SecPerClus = bpb_fat16->BPB_SecPerClus;

            // 6.2 Determine FAT, root, and data LBAs
            // FAT = First sector in partition (boot record) + reserved records
            dsk->fat = dsk->firsts + bpb_fat16->BPB_RsvdSecCnt;

            // 6.3 FATcopy is the number of FAT tables
            dsk->fatcopy = bpb_fat16->BPB_NumFATs;

            // 6.4 MAX ROOT is the maximum number of entries in the root directory
            dsk->maxroot = bpb_fat16->BPB_RootEntCnt;

            // "root" Is the sector location of the root directory
            dsk->root = dsk->fat + (dsk->fatcopy * dsk->fatsize);

            BytesPerSec = bpb_fat16->BPB_BytsPerSec;
            
            if( !BytesPerSec || (BytesPerSec & 1) == 1 )   // Cannot be 0 or odd
             return( CE_NOT_FORMATTED );

            RootDirSectors = ((dsk->maxroot * 32) + (BytesPerSec - 1)) / BytesPerSec;

            // Figure out how many data sectors there are
            DataSec = TotSec - (dsk->root + RootDirSectors);

            dsk->maxcls = DataSec / dsk->SecPerClus;

          // Straight out of MS FAT Hardware White Paper
          if( dsk->maxcls < 4085 ) {
             // Volume is FAT12
             dsk->type = FAT12;
             error = CE_CARDFAT12;   // FAT12 not supported

          } else if( dsk->maxcls < 65525 )  {
             // Volume is FAT16
             dsk->type = FAT16;

          } else {
             // Volume is FAT32
             dsk->type = FAT32;
             error = CE_CARDFAT32;   // FAT32 not supported
          }

          // 6.5 DATA = ROOT + (MAXIMUM ROOT*32 / 512)
          dsk->data = dsk->root + (dsk->maxroot >> 4);   // Assuming maxroot % 16 == 0!!!

          // Make sure that we can read in a complete sector!
          if( BytesPerSec != MEDIA_SECTOR_SIZE )
             error = CE_NOT_FORMATTED;

        }   // Valid Boot Sector signature

    }   // Got Boot Sector

    return( error );

}   // LoadBootSector()


/******************************************************************************
 * Function:        CETYPE DiskMount( DISK *dsk, BYTE *b, MMCSTATE *Flag )
 *
 * PreCondition:    Called from FATMediaInit()
 *
 * Input:           dsk - Pointer to disk to be mounted
 *                  b - Pointer to buffer to be assigned to the disk
 *                  Flag - Pointer to flag structure of card state
 *
 * Output:          CE_GOOD - Disk mounted
 *                  CE_INIT_ERROR - Initialization error has occurred
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to mount the disk/card (only the first
 *                  partition)
 *
 * Note:            Should not be called by user
 *****************************************************************************/
CETYPE DiskMount( UINT idx, DISK *dsk, BYTE *b )
{
    CETYPE error = CE_GOOD;

    dsk->mount  = FALSE;   // Default invalid
    dsk->buffer = b;   // Assign buffer
    dsk->idx    = idx;

    if( (error = LoadMBR( idx, dsk )) == CE_GOOD ) {

       // Now the boot sector
       if( (error = LoadBootSector(idx,  dsk )) == CE_GOOD )
          dsk->mount = TRUE;   // Mark that the disk as mounted successfully
    }

    return( error );

}   // DiskMount()


/******************************************************************************
 * Function:        BOOL FATMediaInit( void )
 *
 * PreCondition:    Called from FAT16Init()
 *
 * Input:           None
 *
 * Output:          TRUE - FAT media initialized successfully
 *                  FALSE - Failure in FAT media initialization
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to initialize the FAT media.
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC BOOL FATMediaInit( UINT pb_clk )
{

    if( MediaInitAll( pb_clk ) != TRUE )
       return( FALSE );

    if( MediaDiskMountAll() != CE_GOOD )
       return( FALSE );

    return( TRUE );

}   // FATMediaInit()

PUBLIC BOOL FATMediaInitIdx( UINT pb_clk, UINT idx )
{

    if( MediaInitIdx( idx, pb_clk ) != TRUE )
       return( FALSE );

    if( MediaDiskMountIdx(idx) != CE_GOOD )
       return( FALSE );

    return( TRUE );

}   // FATMediaInit()

PUBLIC BOOL FATMediaInitName( UINT pb_clk, BYTE *name )
{

    if( MediaInitName( name, pb_clk ) != TRUE )
       return( FALSE );

    if( MediaDiskMountName(name) != CE_GOOD )
       return( FALSE );

    return( TRUE );

}   // FATMediaInit()


/******************************************************************************
 * Function:        WORD FATWrite( DISK *dsk, WORD cls, WORD v )
 *
 * PreCondition:    Disk mounted and valid disk data
 *
 * Input:           dsk - Disk structure
 *                  cls	- Current cluster
 *                  v - New value
 *
 * Output:          c - Cluster value
 *                  FAIL - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to write a cluster link in all
 *                  FAT copies
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC WORD FATWrite( DISK *dsk, WORD cls, WORD v )
{
    BYTE i;
    WORD p, c;
    DWORD l, li;

    if( dsk->type != FAT16 )
       return FAIL;

    gBufferOwner = NULL;

    // Get address of current cluster in fat
    p = cls * 2; // always even

    // cluster = 0xabcd
    // packed as:     0   |    1   |   2   |   3   |
    // word p       0   1 |  2   3 | 4   5 | 6   7 |..
    //              cd  ab|  cd  ab| cd  ab| cd  ab|

    // Load the fat sector containing the cluster
    l = dsk->fat + (p >> 9);
    p &= 0x1ff;
    if( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer ) )
       return FAIL;

    // Get the next cluster value
    RAMwrite( dsk->buffer, p, v);   // lsb
    RAMwrite( dsk->buffer, p+1, (v>>8));   // msb

    // Update all FAT copies
    for( i = 0, li = l; i < dsk->fatcopy; i++, li += dsk->fatsize )
       if( !MediaWriteSectorIdx(dsk->idx,  l, dsk->buffer) )
          return FAIL;

    // Normalize it so 0xFFFF is an error
    if( c >= LAST_CLUSTER_FAT16 )
       c = LAST_CLUSTER;

    return( c );

}   // FATWrite()


/******************************************************************************
 * Function:        WORD FATRead( DISK *dsk, WORD ccls )
 *
 * PreCondition:    None
 *
 * Input:           dsk	- Disk structure
 *                  ccls - Current cluster
 *
 * Output:          0000 - cluster is empty
 *                  0FF8 - last cluster in a chain
 *                  0xxx - next cluster in a chain
 *                  FFFF - FAIL
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to read the content of a cluster
 *                  link in FAT
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC WORD FATRead( DISK *dsk, WORD ccls )
{
    WORD p, c;
    DWORD l;

    if( dsk->type != FAT16 )
       return( CLUSTER_FAIL );

    // Get address of current cluster in FAT
    p = ccls;

    // cluster = 0xabcd
    // packed as:     0   |    1   |   2   |   3   |
    // word p       0   1 |  2   3 | 4   5 | 6   7 |..
    //              cd  ab|  cd  ab| cd  ab| cd  ab|

    // Load the FAT sector containing the cluster
    l = dsk->fat + (p >> 8);   // 256 clusters per sector

    gBufferOwner = NULL;   // Reset buffer owner

    if ( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer ) )
        return( CLUSTER_FAIL );

    // get the next cluster value
    l = (DWORD)p & 0xFF;
    l <<= 1;
    c = RAMreadW( dsk->buffer, l);
//***    c = RAMreadW( dsk->buffer, ((p & 0xFF)<<1));

    // Normalize it so 0xFFFF is an error
    if( c >= LAST_CLUSTER_FAT16 )
       c = LAST_CLUSTER;

    return( c );

}   // FATRead()


/******************************************************************************
 * Function:        WORD FATReadQueued( DISK *dsk, WORD ccls )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           dsk - Disk structure
 *
 * Output:          c - Cluster; 0 if failed
 *
 * Side Effects:    None
 *
 * Overview:        Queue-based FAT read
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC WORD FATReadQueued( DISK *dsk, WORD ccls )
{
    WORD p, c;
    DWORD l;

    if( dsk->type != FAT16 )
       return CLUSTER_FAIL;

        gBufferOwner = NULL;

    // Get address of current cluster in FAT table
    p = ccls;

    // cluster = 0xabcd
    // packed as:     0   |   1    |   2   |   3   |
    // word p       0   1 |  2   3 | 4   5 | 6   7 |..
    //              cd  ab|  cd  ab| cd  ab| cd  ab|

    // Load the FAT sector containing the cluster
    if( !(ccls & 0xFF) ) {
        l = dsk->fat + (p >> 8 );   // 256 clusters per sector
        if( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer) )
            return CLUSTER_FAIL;
    }

    // Get the next cluster value
    c = RAMreadW( dsk->buffer, ((p & 0xFF)<<1) );

    if( c >= LAST_CLUSTER_FAT16 )
       c = LAST_CLUSTER;

    return( c );

}   // FATReadQueued()


/******************************************************************************
 * Function:        WORD FATFindEmptyCluster( FILEOBJ fo )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - Pointer to file structure
 *
 * Output:          c - Cluster; 0 if failed
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to find the next available
 *                  cluster.
 *
 * Note:            None
 *****************************************************************************/
PUBLIC WORD FATFindEmptyCluster( FILEOBJ fo )
{
    DISK *disk;
    WORD value=0x0, c, curcls;

    disk = fo->dsk;
    c = fo->ccls;

    // Just in case
    if( c < 2 )
       c = 2;

    curcls = c;

    FATRead( disk, c );

    // Sequentially scan through the FAT table looking for an empty cluster
    while( c ) {

       // Increment to get to next cluster in FAT table
       c++;

       // Check if reached last cluster in FAT, re-start from top
       if( (value == END_CLUSTER) || (c >= disk->maxcls) )
          c = 2;

       // Check if full circle done, disk full
       if( c == curcls ) {
          c = 0;
          break;
       }

       // Look at its value
       if( (value = FATReadQueued( disk, c )) == CLUSTER_FAIL ) {
          c = 0;
          break;
       }

       // Check if empty cluster found
       if( value == CLUSTER_EMPTY )
          break;

    }   // while loop scanning for an empty cluster

    return( c );

}   // FATFindEmptyCluster()


/******************************************************************************
 * Function:        DWORD Cluster2Sector( DISK *dsk, WORD cluster )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           disk - Pointer to disk structure
 *                  cluster - Cluster number
 *
 * Output:          sector - Sector that corresponds to the cluster
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to figure out the sector number
 *                  based on the cluster number
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC DWORD Cluster2Sector( DISK *dsk, WORD cluster )
{
    DWORD sector;

    // The root dir takes up cluster 0 and 1
    if( !cluster || (cluster == 1) )
       sector = dsk->root + cluster;
    else
       sector = ((cluster-2) * dsk->SecPerClus) + dsk->data;

    return( sector );

}   // Cluster2Sector()


/******************************************************************************
 * Function:        CETYPE EraseCluster( DISK *disk, WORD cluster )
 *
 * PreCondition:    File opened
 *
 * Input:           dsk - Disk structure
 *                  cluster - Cluster to be erased
 *
 * Output:          CE_GOOD - Cluster erased successfully
 *                  CE_WRITE_ERROR - Sector write failed
 *
 * Side Effects:    None
 *
 * Overview:        Erase the passed in cluster
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PUBLIC CETYPE EraseCluster( DISK *disk, WORD cluster )
{
    BYTE index;
    DWORD SectorAddress;
    BYTE error = CE_GOOD;

    SectorAddress = Cluster2Sector( disk, cluster );

    gBufferOwner = NULL;

    // Clear out the memory first
    memset( disk->buffer, 0, MEDIA_SECTOR_SIZE );

    // Now clear them out
    for( index = 0; (index < disk->SecPerClus) &&
                    (error == CE_GOOD); index++ ) {
       if( !MediaWriteSectorIdx(disk->idx, SectorAddress++, disk->buffer ) )
          error = CE_WRITE_ERROR;
    }

    return( error );

}   // EraseCluster()


/******************************************************************************
 * Function:        BOOL FATEraseClusterChain( WORD cluster, DISK *dsk )
 *
 * PreCondition:    Disk mounted, should be called from FileErase()
 *
 * Input:           cluster - The cluster number
 *                  dsk	- The disk structure
 *
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 *
 * Side Effects:    None
 *
 * Overview:        Erase the cluster chain
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL FATEraseClusterChain( WORD cluster, DISK *dsk )
{
    WORD c;
    enum _status{ Good, Fail, Exit } status;

    status = Good;

    // Make sure there is actually a cluster assigned
    if( cluster <= 1 ) {
       status = Exit;

    } else {

       while( status == Good ) {
          // Get the FAT entry
          if( (c = FATRead( dsk, cluster )) == CLUSTER_FAIL )
             status = Fail;
          else {
             if( c <= 1 ) {
                status = Exit;
             } else {
                // Compare against max value of a cluster in FATxx
                // Look for the last cluster in the chain
	        if( c >= LAST_CLUSTER )
	           status = Exit;

                // Now erase this FAT entry
                if( FATWrite( dsk, cluster, CLUSTER_EMPTY ) == (WORD)FAIL )
                   status = Fail;

                // Now update what the current cluster is
                cluster = c;
             }
          }
       }   // while status

    }

    if( status == Exit )
       return( TRUE );
    else
       return( FALSE );

}   // FATEraseClusterChain()

