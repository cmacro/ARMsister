unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, nrclasses, nrcomm, nrcommbox, ComCtrls;

type
  TfrmMain = class(TForm)
    nrDeviceBox1: TnrDeviceBox;
    nrComm1: TnrComm;
    Label1: TLabel;
    Memo1: TMemo;
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
    Button1: TButton;
    Button2: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private    { Private declarations }
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

procedure TfrmMain.WMDeviceChange(var Msg: TMessage);
var i:integer;
begin
  i := nrComm1.DeviceIndex;
  nrComm1.WMDeviceChange(Msg);
  nrComm1.DeviceIndex := i;
  //nrDeviceBox1Change(Self);
end;

procedure TfrmMain.Button1Click(Sender: TObject);
begin
  if not nrComm1.Active then
    UpdateCommParams;

  nrComm1.Active := not nrComm1.Active;
  if nrComm1.Active then
    Button1.Caption := 'Í£Ö¹'
  else
    Button1.Caption := '´ò¿ª'
end;

procedure TfrmMain.UpdateCommParams;
begin
  nrComm1.BaudRate := StrToIntDef(cbxBaudRate.Text, 9600);
  nrComm1.ByteSize := Byte(StrToIntDef(cbxBaudRate.Text, 8));
  nrComm1.Parity := pNone;
  nrComm1.StopBits := sbOne;
  nrComm1.StreamProtocol := spNone;
end;

procedure TfrmMain.Button2Click(Sender: TObject);
begin
  Memo1.clear;
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
      nrComm1.ComPortNo := cFile.ReadInteger('COMM', 'PortNo', 1);
      nrComm1.BaudRate  := Cardinal(cFile.ReadInteger('COMM', 'BaudRate', 9600));
      nrComm1.ByteSize  := Byte(cFile.ReadInteger('COMM', 'ByteSize', 8));
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
    cFile.WriteInteger('COMM', 'PortNo', nrComm1.ComPortNo);
    cFile.WriteInteger('COMM', 'BaudRate', nrComm1.BaudRate);
    cFile.WriteInteger('COMM', 'ByteSize', nrComm1.ByteSize);
  finally
    cFile.Free;
  end;
end;

procedure TfrmMain.FormCreate(Sender: TObject);
begin
  ReadConfigs;
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
begin
  WriteConfigs;
end;

end.
