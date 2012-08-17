/******************************************************************************
  Target Script for Philips LPC2000.

  Copyright (c) 2005 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function Reset()
{
  TargetInterface.resetAndStop(50);
  TargetInterface.getICEBreakerRegister(5); /* Clear out Debug Comms Data */
}

function LPC210X_ResetWithSRAMVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 2);
}

function LPC210X_ResetWithFLASHVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 1);
}

function LPC210X_ResetWithBootLoaderFLASHVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 0);
}

function SRAMReset()
{
  LPC210X_ResetWithSRAMVectorsAtZero();
}

function FLASHReset()
{
  LPC210X_ResetWithFLASHVectorsAtZero()
}

