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
using System.Windows.Forms;
using System.Drawing;
using Microsoft.Win32;

namespace PersistentForms
{
	/// <summary>
	/// Zusammenfassung für PersistentForm.
	/// </summary>
	public class PersistentForm
	{
		private System.Windows.Forms.Form		mainFrame;
		private ViewProfile						profile;
		private bool							restoreComplete = false;
		private bool							bSaveComponents = false;

		private bool SaveComponents
		{
			get
			{
				return bSaveComponents;
			}
			set
			{
				bSaveComponents = value;
			}
		}

		public PersistentForm( System.Windows.Forms.Form i_mainFrame, string i_subkey )
		{
			mainFrame = i_mainFrame;
			profile = new ViewProfile( i_subkey );

			// the intercepted eventhandlers
			mainFrame.Closing += new System.ComponentModel.CancelEventHandler(this.MainFrame_Closing);
			mainFrame.Load += new System.EventHandler(this.MainFrame_Load);
			mainFrame.Resize += new System.EventHandler(this.MainFrame_Resize);
			mainFrame.Move += new System.EventHandler(this.MainFrame_Move);
		}

		/// <summary>
		/// Restores windows state and position from registry.
		/// </summary>
		public void Restore()
		{
			try
			{
				restoreComplete = false;
				mainFrame.DesktopBounds = Profile.ReadRect( Names.strBounds + mainFrame.Name );
				mainFrame.WindowState = (FormWindowState)Profile.ReadInt( Names.strWindowState + mainFrame.Name );
				if ( SaveComponents )
				{
					foreach ( Control control in mainFrame.Controls )
					{
						RestoreControl(control);
					}
				}
			}
			catch(Exception)
			{
			}
			finally
			{
				restoreComplete = true;
			}
		}

		/// <summary>
		/// Saves windows state and position under registry.
		/// </summary>
		public void Save()
		{
			if ( this.restoreComplete )
			{
				Profile.Write( Names.strBounds + mainFrame.Name, mainFrame.DesktopBounds );
				SaveWindowState();
				if ( SaveComponents )
				{
					foreach( Control control in mainFrame.Controls )
					{
						SaveControl(control);
					}
				}
			}
		}

		private void SaveWindowState()
		{
			Profile.Write( Names.strWindowState + mainFrame.Name, (int)mainFrame.WindowState );
		}
        
		private void SaveColumnsOnly()
		{
			foreach(Control control in mainFrame.Controls)
			{
				SaveColumns( control );
			}
		}

		private ViewProfile Profile
		{
			get
			{
				return profile;
			}
		}

		private void RestoreControl(Control control)
		{
			Type controlType = control.GetType();
			bool isToolBar = (controlType == typeof(ToolBar));
			bool isStatusBar = (controlType == typeof(StatusBar));
			if ( !isToolBar && !isStatusBar)
			{
				control.Bounds = Profile.ReadRect( Names.strBounds + control.Name );
			}
			bool isListView = (controlType == typeof(ListView));
			if ( isListView )
			{
				RestoreColumns((ListView)  control );
			}
		}

		private void SaveControl(Control control)
		{
			Type controlType = control.GetType();
			bool isToolBar = (controlType == typeof(ToolBar));
			bool isStatusBar = (controlType == typeof(StatusBar));
			if ( !isToolBar && !isStatusBar )
			{
				Profile.Write( Names.strBounds + control.Name, control.Bounds );
			}			
			SaveColumns(control);
		}

		private void RestoreColumns(ListView listView)
		{
			foreach ( ColumnHeader column in listView.Columns )
			{
				column.Width = Profile.ReadInt(Names.strColumn + column.Index);
			}
		}

		private void SaveColumns(Control control)
		{
			Type controlType = control.GetType();
			bool isListView = (controlType == typeof(ListView));
			if ( isListView )
			{
				SaveColumns( (ListView) control );
			}
		}

		private void SaveColumns(ListView listView)
		{
			foreach ( ColumnHeader column in listView.Columns )
			{
				Profile.Write( Names.strColumn + column.Index, column.Width );
			}
		}

		private void MainFrame_Closing( object sender, System.ComponentModel.CancelEventArgs e )
		{
			if ( mainFrame.WindowState == FormWindowState.Normal )
			{
				Save();
			}
			else
			{
				SaveColumnsOnly();
			}
		}

		private void MainFrame_Load( object sender, System.EventArgs e )
		{
			Restore();
		}

		private void MainFrame_Resize( object sender, System.EventArgs e )
		{
			if ( mainFrame.WindowState == FormWindowState.Normal )
			{
				Save();
			}
			else
			{
				SaveWindowState();
			}
		}

		private void MainFrame_Move( object sender, System.EventArgs e )
		{
			if ( mainFrame.WindowState == FormWindowState.Normal )
			{
				Save();
			}
			else
			{
				SaveWindowState();
			}
		}	
	}
}
