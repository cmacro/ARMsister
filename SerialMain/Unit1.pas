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
    procedure Button1Click(Sender: TObject);
  private    { Private declarations }
    procedure UpdateCommParams;
  public     { Public declarations }
    procedure WMDeviceChange(var Msg:TMessage); message WM_DEVICECHANGE;      
  end;

var
  frmMain: TfrmMain;

implementation

uses
  nrwin32di, nrwin32;

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

end.
