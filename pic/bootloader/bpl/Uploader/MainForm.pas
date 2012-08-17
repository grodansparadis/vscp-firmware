unit MainForm;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Buttons, AdPort, ExtCtrls, OoMisc, ComCtrls, Registry;

type
  ByteBuffer = array[0..128] of Byte;
  TfrmMain = class(TForm)
    meLog: TMemo;
    cbSpeed: TComboBox;
    Label1: TLabel;
    cbPort: TComboBox;
    Label2: TLabel;
    bbUpload: TBitBtn;
    ComPort: TApdComPort;
    TimeoutTicker: TTimer;
    ProgressBar: TProgressBar;
    odOpen: TOpenDialog;
    bbOpen: TBitBtn;
    edFilename: TEdit;
    Label3: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure bbUploadClick(Sender: TObject);
    procedure TimeoutTickerTimer(Sender: TObject);
    procedure SendPacket(PacketType : Byte; LoadAddress : Word;
                         Buffer : ByteBuffer; BufLen : Byte);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure bbOpenClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.DFM}

var
  HexCode       : array[0..$7FFF] of Byte;
  HexData       : array[0..$FF] of Byte;
  HexConfig     : array[0..$F] of Byte;
  HexAddress    : LongInt = 0;
  HexLastCode   : LongInt = -1;
  HexLastData   : LongInt = -1;
  HexLastConfig : LongInt = -1;
const
  ERR_NOTHEX = 1;
  ERR_HEXCHECK = 2;
  ERR_HEXEOF = 3;

const
  CMD_CODEWRITE = 0;
  CMD_DATAWRITE = 1;
  CMD_IDENTIFY  = 2;
  CMD_DATAREAD  = 3;
  CMD_RESET     = 4;

function DecodeHex(var fp : Text) : Byte; forward;

var
  Countdown : Integer;

procedure TfrmMain.FormCreate(Sender: TObject);
var
  r : TRegistry;
begin
  try
    // Open the registry key
    r := TRegistry.Create;
    r.OpenKey('Software\Phil Pemberton\P2B PIC Bootloader', True);

    // Restore the comms settings and the last file that was loaded
    cbPort.ItemIndex := r.ReadInteger('ComPort') - 1;
    cbSpeed.ItemIndex := r.ReadInteger('BaudRate');
    edFilename.Text := r.ReadString('MRUFile');

    // Restore the window position
    Self.Left := r.ReadInteger('WindowLeft');
    Self.Top := r.ReadInteger('WindowTop');

    // Make sure the window doesn't go off the screen
    if (Self.Left >= (Screen.Width - Self.Width)) then
      Self.Left := Screen.Width - Self.Width;
    if (Self.Top >= (Screen.Height - Self.Height)) then
      Self.Top := Screen.Height - Self.Height;

    r.Free;
  except
    cbPort.ItemIndex := 0;
    cbSpeed.ItemIndex := 4;
  end;
end;

procedure TfrmMain.FormClose(Sender: TObject; var Action: TCloseAction);
var
  r : TRegistry;
begin
  // Save the settings
  try
    r := TRegistry.Create;
    r.OpenKey('Software\Phil Pemberton\P2B PIC Bootloader', True);
    r.WriteInteger('ComPort', cbPort.ItemIndex+1);
    r.WriteInteger('BaudRate', cbSpeed.ItemIndex);
    r.WriteString('MRUFile', edFilename.Text);

    // save the window position
    r.WriteInteger('WindowLeft', Self.Left);
    r.WriteInteger('WindowTop', Self.Top); 
    r.Free;
  except
  end;
end;

procedure TfrmMain.TimeoutTickerTimer(Sender: TObject);
begin
  Dec(Countdown);
  if Countdown = 0 then TimeoutTicker.Enabled := False;
end;

procedure TfrmMain.bbUploadClick(Sender: TObject);
var
  Flag    : Boolean;
  ch      : Char;
  Buffer  : ByteBuffer;
  Loop    : Integer;
  Segment : LongInt;
  fp      : System.Text;
  code    : Byte;
begin
  if edFilename.Text <> '' then
  begin
    // Clear the data storage buffers
    FillChar(HexCode, sizeof(HexCode), 255);
    FillChar(HexData, sizeof(HexData), 255);
    FillChar(HexConfig, sizeof(HexConfig), 255);
    // Clear the log
    meLog.Lines.Clear;
    // Open the hex file
    FileMode := 0;
    {$I-}
    AssignFile(fp, edFilename.Text);
    Reset(fp);
    {$I+}
    // Was there an error?
    if (IOResult <> 0) then
    begin
      MessageDlg('Error opening hex file.', mtError, [mbOK], 0);
      Exit;
    end;

    // Prepare to load it
    loop := 1;
    code := 0;
    // Load the hex file into memory
    while ((not EOF(fp)) and (code = 0)) do
    begin
      code := DecodeHex(fp);
//      meLog.Lines.Add('decode line ' + IntToStr(loop) + ' = ' + IntToStr(code));
      inc(loop);
    end;
    // Display some statistics
    meLog.Lines.Add('-- READHEX exited with code ' + IntToStr(Code) + ' at line ' + IntToStr(loop));
//    meLog.Lines.Add('last code byte   = ' + IntToHex(HexLastCode, 8));
//    meLog.Lines.Add('last data byte   = ' + IntToHex(HexLastData, 8));
//    meLog.Lines.Add('last config byte = ' + IntToHex(HexLastConfig, 8));

    // close the hex file
    CloseFile(fp);

    // Was there an error?
    if (code <> ERR_HEXEOF) then
    begin
      MessageDlg('Error loading hex file (code ' + IntToStr(code) + ')', mtError, [mbOK], 0);
      Exit;
    end;

    // try and contact the loader
    meLog.Lines.Add('Opening COM port');
    try
      // Open the COM port
      ComPort.ComNumber := cbPort.ItemIndex + 1;
      ComPort.Baud := StrToInt(cbSpeed.Text);
      ComPort.Open := True;
    except
      // Oh dear. Bad things have happened
      meLog.Lines.Add('Error opening comm port');
      Exit;
    end;

    // Wait for the loader to make a request
    Countdown := 5;                        // Five second timeout
    TimeoutTicker.Enabled := True;
    ComPort.FlushInBuffer;                  // Flush the COM buffer
    // Set up the progress bar
    ProgressBar.Min := 0;
    ProgressBar.Position := 0;
    ProgressBar.Max := 5;

    Flag := False;
    meLog.Lines.Add('Waiting for bootloader...');
    repeat
      // Wait for a character
      if ComPort.CharReady then
        // Make sure it's a "H"
        if ComPort.GetChar = 'H' then
        begin
          // Send an ACK
          ComPort.PutChar('i');
          // Wait for a "K" (ACK)
          Countdown := 5;
          ch := Chr(0);
          repeat
            Application.ProcessMessages;
            if ComPort.CharReady then
              ch := ComPort.GetChar;
          until (ch = 'K') or (Countdown = 0);
          if (ch = 'K') then Flag := True;
        end;
      ProgressBar.Position := 5-Countdown;
      Application.ProcessMessages;
    until (Countdown = 0) or (Flag);
    ProgressBar.Position := ProgressBar.Max;

    // See if the bootloader is alive
    if Flag then
      meLog.Lines.Add('Bootloader is ALIVE!')
    else begin
      meLog.Lines.Add('No response from bootloader. Abort.');
      Exit;
    end;

    // Bootloader is alive. Grab the device ID
    ComPort.FlushInBuffer;
    SendPacket(CMD_IDENTIFY, 0, Buffer, 0);
    // Read the response packet
    for loop := 0 to 5 do
    begin
      // TODO: read timeout
      repeat Application.ProcessMessages until ComPort.CharReady;
      Buffer[loop] := Ord(ComPort.GetChar);
    end;

    meLog.Lines.Add(Format('Device ID: %02X:%02X   Loader version %s.%s%s',
                           [Buffer[2], Buffer[1], Chr(Buffer[3]),
                            Chr(Buffer[4]), Chr(Buffer[5])]));

    // Is there any code to program?
    if (HexLastCode <> -1) then
    begin
      // Set up the progress bar
      ProgressBar.Min := 0;
      ProgressBar.Max := (((HexLastCode + 15) div 16)-1);
      ProgressBar.Position := 32;
      ProgressBar.Min := 32;
      // Upload the code to FLASH
      for Segment := 32 to (((HexLastCode + 15) div 16)-1) do
      begin
        // Flush the input buffer
        ComPort.FlushInBuffer;
        // Send the data
        Move(HexCode[segment*16], Buffer, 16);
        SendPacket(CMD_CODEWRITE, (Segment*16), Buffer, 16);
        // Read the response byte
        repeat until ComPort.CharReady;
        code := Ord(ComPort.GetChar);
        if (code <> 0) then
        begin
          meLog.Lines.Add('Error programming CODE area ' + IntToHex(Segment*16, 8) +
                          '; error code ' + IntToStr(code));
          ComPort.Open := False;
          Exit;
        end;
        ProgressBar.Position := Segment;
        Application.ProcessMessages;
      end;
    end else
    begin
      meLog.Lines.Add('No code to program; skipping...');
    end;

    // Now do the same for the data EEPROM
    if (HexLastData <> -1) then
    begin
      // Set up the progress bar
      ProgressBar.Min := 0;
      ProgressBar.Position := 0;
      ProgressBar.Max := (((HexLastData + 15) div 16)-1);
      // Upload the code to FLASH
      for Segment := 0 to (((HexLastData + 15) div 16)-1) do
      begin
        // Send the data
        Move(HexData[segment*16], Buffer, 16);
        SendPacket(CMD_DATAWRITE, (Segment*16), Buffer, 16);
        // Read the response byte
        repeat until ComPort.CharReady;
        code := Ord(ComPort.GetChar);
        if (code <> 0) then
        begin
          meLog.Lines.Add('Error programming; error code ' + IntToStr(code));
          ComPort.Open := False;
          Exit;
        end;
        ProgressBar.Position := Segment;
        Application.ProcessMessages;
      end;
    end else
    begin
      meLog.Lines.Add('No data EEPROM to program; skipping...');
    end;

    // Now reset the chip
    SendPacket(CMD_RESET, 0, Buffer, 0);
  end else
  begin
    meLog.Lines.Add('No file selected');
  end;

  try
    meLog.Lines.Add('Finished.');
    ComPort.Open := False;
  except
  end;
end;

////////////////////////////////////////////////////////////////////////////////

procedure TfrmMain.SendPacket(PacketType : Byte; LoadAddress : Word;
                              Buffer : ByteBuffer; BufLen : Byte);
var
  Loop : Word;
  Checksum : Byte;
begin
  // Send the packet type
  ComPort.PutChar(Chr(PacketType));
  // Send the load address
  ComPort.PutChar(Chr(LoadAddress shr 16));     // MSB...
  ComPort.PutChar(Chr(LoadAddress shr 8));
  ComPort.PutChar(Chr(LoadAddress and $FF));    // ...to LSB
  // Send the data length byte
  ComPort.PutChar(Chr(BufLen));
  // Prepare for checksum calculation
  Checksum := 0;
  // Any data to send?
  if (BufLen > 0) then
  begin
    for Loop := 0 to BufLen-1 do
    begin
      ComPort.PutChar(Chr(Buffer[Loop]));
      Checksum := Byte(Checksum + Buffer[Loop]);
    end;
    // Final checksum calculation
    Checksum := (not Checksum) + 1;
    // Send the checksum
    ComPort.PutChar(Chr(Checksum));
  end;
end;

////////////////////////////////////////////////////////////////////////////////

function HexToInt(inp : string) : Byte;
const
  HexTable : string = '0123456789ABCDEF';
var
  hi, lo : Byte;
begin
  if (Length(inp) < 2) then
  begin
    Result := 0;
    Exit;
  end;
  hi := Pos(inp[1], HexTable);
  lo := Pos(inp[2], HexTable);
  if ((hi = 0) or (lo = 0)) then
  begin
    Result := 0;
    Exit;
  end;
  Result := ((hi-1) shl 4) or (lo-1);
end;

function DecodeHex(var fp : Text) : Byte;
var
  numDataBytes : Byte;
  lowAddress   : Word;
  highAddress  : Word;
  RecordType   : Byte;
  Checksum     : Byte;
  OurChecksum  : Byte;
  Loop         : Byte;
  DataByte     : Byte;
  LineBuffer   : string;
begin
  // Read a line from the file
  ReadLn(fp, LineBuffer);
  // Is it a valid hex record?
  if ((Length(LineBuffer) < 1) or (LineBuffer[1] <> ':')) then
  begin
    result := ERR_NOTHEX;
    Exit;
  end;

  // Is it an EOF marker?
  if (LineBuffer = ':00000001FF') then
  begin
    Result := ERR_HEXEOF;
    Exit;
  end;

  // How many bytes on this line?
  numDataBytes := HexToInt(LineBuffer[2] + LineBuffer[3]);
  ourChecksum := numDataBytes;
  // Address
  lowAddress := (HexToInt(LineBuffer[4] + LineBuffer[5]) shl 8) or
                 HexToInt(LineBuffer[6] + LineBuffer[7]);
  ourChecksum := byte(ourChecksum + HexToInt(LineBuffer[4] + LineBuffer[5]) +
                 HexToInt(LineBuffer[6] + LineBuffer[7]));

  // Record type
  RecordType := HexToInt(LineBuffer[8] + LineBuffer[9]);
  ourChecksum := ourChecksum + RecordType;

  case RecordType of
    0:            // data record
      begin
        // update the load address
        HexAddress := (HexAddress and $FFFF0000) or lowAddress;
        for loop := 0 to NumDataBytes-1 do
        begin
          // read a data byte
          DataByte := HexToInt(LineBuffer[10+(loop*2)] + LineBuffer[11+(loop*2)]);
          // store it in the correct buffer
          if (((loop + HexAddress) >= $0000) and
              ((loop + HexAddress) <= $7FFF)) then
            HexCode[loop+HexAddress] := DataByte
          else if (((loop + HexAddress) >= $300000) and
                   ((loop + HexAddress) <= $30000F)) then
            HexConfig[loop+HexAddress-$300000] := DataByte
          else if (((loop + HexAddress) >= $F00000) and
                   ((loop + HexAddress) <= $F000FF)) then
            HexData[loop+HexAddress-$F00000] := DataByte;
          // update the checksum
          ourChecksum := byte(ourChecksum + DataByte);
        end;
        // store the address of the last byte of memory
        if (((HexAddress + NumDataBytes - 1) <= $7FFF) and
            ((HexAddress + NumDataBytes - 1) >= $000000) and
            ((HexAddress + NumDataBytes - 1) > HexLastCode)) then
          HexLastCode := HexAddress + NumDataBytes - 1

        else if (((HexAddress + NumDataBytes - 1) <= $30000F) and
                ((HexAddress + NumDataBytes - 1) >= $300000) and
            ((HexAddress + NumDataBytes - $3FFFF1) > HexLastConfig)) then
          HexLastConfig := HexAddress + NumDataBytes - $300001

        else if (((HexAddress + NumDataBytes - 1) <= $F000FF) and
                ((HexAddress + NumDataBytes - 1) >= $F00000) and
            ((HexAddress + NumDataBytes - $F00001) > HexLastData)) then
          HexLastData := HexAddress + NumDataBytes - $F00001;

        // get the checksum
        Checksum := HexToInt(LineBuffer[10+(NumDataBytes*2)] + LineBuffer[11+(NumDataBytes*2)]);
        // test the checksum
        if (((ourChecksum + Checksum) and $ff) <> 0) then
        begin
          Result := ERR_HEXCHECK;
          Exit;
        end;
      end;
    4:            // linear address
      begin
        // upper 16 bits of address
        highAddress := (HexToInt(LineBuffer[10] + LineBuffer[11]) shl 8) or
                        HexToInt(LineBuffer[12] + LineBuffer[13]);
        HexAddress := lowAddress or (highAddress shl 16);
        // get the checksum
        OurChecksum := OurChecksum + HexToInt(LineBuffer[10] + LineBuffer[11]) +
            HexToInt(LineBuffer[12] + LineBuffer[13]);
        Checksum := HexToInt(LineBuffer[14] + LineBuffer[15]);
        // test the checksum
        if ((ourChecksum + Checksum) <> 256) then
        begin
          Result := ERR_HEXCHECK;
          Exit;
        end;
      end;
    else          // something else
      begin
        Result := ERR_NOTHEX;
        exit;
      end;
  end;
  Result := 0;
end;

procedure TfrmMain.bbOpenClick(Sender: TObject);
begin
  if odOpen.Execute then
    edFilename.Text := odOpen.FileName;
end;

end.
