using System;
using System.IO;

namespace Tracer
{
	/// <summary>C
	/// Zusammenfassung für Tracer.
	/// </summary>
	public class CTracer
	{
		private string			m_fileName;				// the filename to be used
		private bool			m_traceEnabled;			// trace enabled ?
		private StreamWriter	m_textWriter;

		// private string			m_sEndln = "\r\n";			// newline 

		public CTracer( string i_fileName, bool i_bEnabled )
		{
			//
			// TODO: Fügen Sie hier die Konstruktorlogik hinzu
			//
			m_fileName = i_fileName;
			m_traceEnabled = i_bEnabled;
			if ( m_traceEnabled )
			{
				m_textWriter = new System.IO.StreamWriter( m_fileName );
			}

		}

		public void	Write( string i_trcString )
		{
			if ( m_traceEnabled )
			{
				m_textWriter.WriteLine( i_trcString );
				m_textWriter.Flush();
			}
		}
		public void WriteTimestamp( string i_trcString )
		{
			if ( m_traceEnabled )
			{
				DateTime now = DateTime.Now;
				//string FormatString1 = String.Format("{0:dddd MMMM}", DateTime.Now);
				string sText = now.ToLongTimeString();
				sText = sText + ":" + now.Millisecond;
				m_textWriter.WriteLine( sText + " " + i_trcString );
				m_textWriter.Flush();
			}
		}
	}
}
