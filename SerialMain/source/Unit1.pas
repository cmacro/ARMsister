unit Unit1;

interface

{$define BuildComm}

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, nrclasses, nrcomm, nrcommbox, ComCtrls, Menus;

type
  TfrmMain = class(TForm)
    nrDeviceBox1: TnrDeviceBox;
    Label1: TLabel;
    memLog: TMemo;
    cbxBaudRate: TComboBox;
    ComboBox2: TComboBox;
    ComboBox3: TComboBox;
    ComboBox4: TComboBox;
    ComboBox5: TComboBox;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    btnActive: TButton;
    btnClean: TButton;
    PopupMenu1: TPopupMenu;
    miSaveLog: TMenuItem;
    procedure btnActiveClick(Sender: TObject);
    procedure btnCleanClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure miSaveLogClick(Sender: TObject);
  private    { Private declarations }
    FComm: TnrComm;
    procedure UpdateCommParams;
    function  GetConfigFile: string;
    procedure ReadConfigs;
    procedure WriteConfigs;
  public     { Public declarations }
    procedure WMDeviceChange(var Msg:TMessage); message WM_DEVICECHANGE;      
  end;

var
  frmMain: TfrmMain;

implementation

uses
  nrwin32di, nrwin32, IniFiles;

{$R *.dfm}

function GetFileVersion(const FileName: string): string;
var
  pInfo: PVSFixedFileInfo;
  pBuf: pointer;
  iSize: Cardinal;
  dump: Cardinal;
begin
  Result:='';
  iSize := GetFileVersionInfoSize(PChar(FileName), dump);
  if iSize <> 0 then
  begin
    try
      GetMem(pBuf, iSize);
      try
        GetFileVersionInfo(PChar(FileName), 0, iSize, pBuf);
        if VerQueryValue(pBuf, '\', Pointer(pInfo), dump) then
          Result := format('%d.%d.%d.%d', [
                    HIWORD(pInfo.dwFileVersionMS),
                    LOWORD(pInfo.dwFileVersionMS),
                    HIWORD(pInfo.dwFileVersionLS),
                    LOWORD(pInfo.dwFileVersionLS)
                    ])
      finally
        FreeMem(pBuf);
      end;
    except
      Result := '-1';
    end;
  end;
end;

function GetAppVersion:String;
begin
  Result:=GetFileVersion(Application.ExeName);
end;

procedure TfrmMain.WMDeviceChange(var Msg: TMessage);
var i:integer;
begin
  i := FComm.DeviceIndex;
  FComm.WMDeviceChange(Msg);
  FComm.DeviceIndex := i;
  //nrDeviceBox1Change(Self);
end;

procedure TfrmMain.btnActiveClick(Sender: TObject);
begin
  if not FComm.Active then
    UpdateCommParams;

  FComm.Active := not FComm.Active;
  if FComm.Active then
    btnActive.Caption := '停止'
  else
    btnActive.Caption := '打开'
end;

procedure TfrmMain.UpdateCommParams;
begin
  FComm.BaudRate := StrToIntDef(cbxBaudRate.Text, 9600);
  FComm.ByteSize := Byte(StrToIntDef(cbxBaudRate.Text, 8));
  FComm.Parity := pNone;
  FComm.StopBits := sbOne;
  FComm.StreamProtocol := spNone;
end;

procedure TfrmMain.btnCleanClick(Sender: TObject);
begin
  memLog.clear;
end;

function DirJoint(const AName, ARoot: string):string;
begin
  Assert(ARoot <> '');
  if ARoot[Length(ARoot)] = '\' then
    Result := format('%s%s', [ARoot, AName])
  else
    Result := format('%s\%s', [ARoot, AName]);
end;

function TfrmMain.GetConfigFile:string;
var
  sPath :string;
begin
  sPath := GetEnvironmentVariable('APPDATA');
  if DirectoryExists(sPath) then
    sPath := DirJoint('ARMsister', sPath)
  else
    sPath := ExtractFilePath(ParamStr(0));
    
  Result := DirJoint('Config.ini', sPath);
end;

procedure TfrmMain.ReadConfigs;
var
  cFile: TIniFile;
  sFileName: string;
begin
  sFileName := GetConfigFile;
  if FileExists(sFileName) then
  begin
    cFile := TIniFile.Create(sFileName);
    try
      FComm.ComPortNo := cFile.ReadInteger('COMM', 'PortNo', 1);
      FComm.BaudRate  := Cardinal(cFile.ReadInteger('COMM', 'BaudRate', 9600));
      FComm.ByteSize  := Byte(cFile.ReadInteger('COMM', 'ByteSize', 8));
    finally
      cFile.Free;
    end;
  end;
end;

procedure TfrmMain.WriteConfigs;
var
  cFile: TIniFile;
  sFileName: string;
begin
  UpdateCommParams;
  sFileName := GetConfigFile;
  if not DirectoryExists(ExtractFilePath(sFileName)) then
    CreateDir(ExtractFilePath(sFileName));

  cFile := TIniFile.Create(sFileName);
  try
    cFile.WriteInteger('COMM', 'PortNo', FComm.ComPortNo);
    cFile.WriteInteger('COMM', 'BaudRate', FComm.BaudRate);
    cFile.WriteInteger('COMM', 'ByteSize', FComm.ByteSize);
  finally
    cFile.Free;
  end;
end;

procedure TfrmMain.FormCreate(Sender: TObject);
var
  sVer: string;
begin
  sVer := GetAppVersion;
  if sVer <> '' then
    Caption := format('%s V:%s', [Caption, sVer]);

{$ifdef BuildComm}
  FComm := TnrComm.Create(Self);
  with FComm do
  begin
    EnumPorts := epFullPresent;

    BaudRate := 9600;
    ByteSize := 8;
    Parity := pNone;
    StopBits := sbOne;
    StreamProtocol := spNone;
    RS485Mode := False;

    TraceStates := [];
    EventChar := #0;
    TimeoutRead := 0;
    TimeoutWrite := 100;
    UseMainThread := True;
    TerminalUsage := tuReceive;
    TerminalEcho := False;
  end;
{$endif}

  nrDeviceBox1.nrComm := FComm;
  FComm.Terminal := memLog;

  ReadConfigs;
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
begin
  WriteConfigs;
  {$ifdef BuildComm}
  FComm.Free;
  {$endif}
end;

procedure TfrmMain.miSaveLogClick(Sender: TObject);
var
  cDlg: TSaveDialog;
  sFileName: string;
begin
  sFileName := '';
  cDlg := TSaveDialog.Create(Self);
  try
    cDlg.Filter := '日志文件(*.log)|*.log|文本文件(*.txt)|*.txt|所有文件(*.*)|*.*';
    cDlg.FilterIndex := 0;
    cDlg.DefaultExt := 'log';
    if cDlg.Execute then
      sFileName := cDlg.FileName;
  finally
    cDlg.Free;
  end;

  if sFileName <> '' then
    memLog.Lines.SaveToFile(sFileName);
end;

end.
