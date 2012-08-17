/// 
/// ***************************************************************************
/// Copyright (c) 2007 and beyond Robert M. Bouwens, CH-5425 Schneisingen, Switzerland. Allrights reserved.
/// 
/// Written by Robert M. Bouwens, 
/// 
/// This code is free for private use.
/// Use in commercial applications requires written permission
/// This software is provided "as is", with no warranty.
/// 
/// For comments mailto:bouwens.mehl@bluewin.com
///
using System;
using Microsoft.Win32;
using System.Drawing;

namespace PersistentForms
{
	public class Names
	{
		public static string strBounds = "Bounds";
		public static string strColumn = "Column";
		public static string strWindowState = "WindowState";
		public static string strX = "X";
		public static string strY = "Y";
		public static string strWidth = "Width";
		public static string strHeight = "Height";
		public static string strSoftware = "Software\\";
	}
	/// <summary>
	/// Zusammenfassung für ViewProfile.
	/// Read and write the registry keys
	/// </summary>
	public class ViewProfile
	{
		protected ViewProfile()
		{
			//
			// TODO: Fügen Sie hier die Konstruktorlogik hinzu
			//
		}
		private string subkey;
		public ViewProfile( string subkey )
		{
			this.subkey = subkey;
		}

		
		public Rectangle ReadRect( string fieldName )
		{
			Rectangle rect = new Rectangle();
			rect.X = ReadInt(fieldName + Names.strX);
			rect.Y = ReadInt(fieldName + Names.strY);
			rect.Width = ReadInt(fieldName + Names.strWidth);
			rect.Height = ReadInt(fieldName + Names.strHeight);
			return rect;
		}

		public void Write(string fieldName, Rectangle rect)
		{
			Write(fieldName + Names.strX, rect.X);
			Write(fieldName + Names.strY, rect.Y);
			Write(fieldName + Names.strWidth, rect.Width);
			Write(fieldName + Names.strHeight, rect.Height);
		}

		public int ReadInt(string fieldName)
		{
			RegistryKey key = Registry.CurrentUser.OpenSubKey( strSubKey );
			int val = (int)key.GetValue(fieldName);
			return val;
		}

		public void Write(string fieldName, int value)
		{
			RegistryKey key = Registry.CurrentUser.CreateSubKey( strSubKey );
			key.SetValue(fieldName, value);
		}

		public string strSubKey
		{
			get
			{
				return Names.strSoftware + subkey;
			}
		}
	}
}
