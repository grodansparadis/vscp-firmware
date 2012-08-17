program Uploader;

uses
  Forms,
  MainForm in 'MainForm.pas' {frmMain};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'Philpem''s PIC Bootloader';
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.
