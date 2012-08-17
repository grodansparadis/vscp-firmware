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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace BRM_Tools
{
    public partial class SafeListView : ListView
    { 
        public delegate void GenericEventHandler<T>(T t);

        private int m_elems;

        public SafeListView()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
            this.m_elems = 0;
        }

        /// <summary>
        /// Threadsafe method to add a row in a listview
        /// </summary>
        /// <param name="item"></param>
        public void AddItem(ListViewItem i_item)
        {
            if (InvokeRequired)
            {
                // different thread or gui is not ready
                GenericEventHandler<ListViewItem> setItemDel = delegate(ListViewItem item_del)
                                                          {
                                                              this.AddItem(item_del);
                                                          };
                try
                {
                    Invoke(setItemDel, new object[] { i_item });
                }
                catch { /* na ja ... */ }
            }
            else
            {
                ListViewItem item = base.Items.Add(i_item);
                item.EnsureVisible();
            }
        }
    }
}
