object frmMain: TfrmMain
  Left = 248
  Top = 232
  Width = 606
  Height = 409
  Caption = #20018#21475#35843#35797' '#23567#31302#29645#34255#29256
  Color = clBtnFace
  Constraints.MinHeight = 250
  Constraints.MinWidth = 440
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = #24494#36719#38597#40657
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  DesignSize = (
    590
    371)
  PixelsPerInch = 96
  TextHeight = 19
  object Label1: TLabel
    Left = 34
    Top = 35
    Width = 42
    Height = 19
    Caption = #20018'    '#21475
  end
  object Label2: TLabel
    Left = 34
    Top = 75
    Width = 39
    Height = 19
    Caption = #27874#29305#29575
  end
  object Label3: TLabel
    Left = 34
    Top = 107
    Width = 39
    Height = 19
    Caption = #25968#25454#20301
  end
  object Label4: TLabel
    Left = 34
    Top = 139
    Width = 39
    Height = 19
    Caption = #26657#39564#20301
  end
  object Label5: TLabel
    Left = 34
    Top = 171
    Width = 39
    Height = 19
    Caption = #20572#27490#20301
  end
  object Label6: TLabel
    Left = 34
    Top = 203
    Width = 39
    Height = 19
    Caption = #27969#25511#21046
  end
  object nrDeviceBox1: TnrDeviceBox
    Left = 96
    Top = 32
    Width = 466
    Height = 27
    nrComm = nrComm1
    ResetOnChanged = False
    ItemHeight = 19
    TabOrder = 0
    Anchors = [akLeft, akTop, akRight]
  end
  object Memo1: TMemo
    Left = 264
    Top = 72
    Width = 298
    Height = 272
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 1
  end
  object cbxBaudRate: TComboBox
    Left = 96
    Top = 72
    Width = 145
    Height = 27
    ItemHeight = 19
    TabOrder = 2
    Text = '9600'
  end
  object ComboBox2: TComboBox
    Left = 96
    Top = 104
    Width = 145
    Height = 27
    ItemHeight = 19
    TabOrder = 3
    Text = '8'
  end
  object ComboBox3: TComboBox
    Left = 96
    Top = 136
    Width = 145
    Height = 27
    ItemHeight = 19
    TabOrder = 4
    Text = 'None'
  end
  object ComboBox4: TComboBox
    Left = 96
    Top = 168
    Width = 145
    Height = 27
    ItemHeight = 19
    TabOrder = 5
    Text = '1'
  end
  object ComboBox5: TComboBox
    Left = 96
    Top = 200
    Width = 145
    Height = 27
    ItemHeight = 19
    TabOrder = 6
    Text = 'None'
  end
  object Button1: TButton
    Left = 96
    Top = 256
    Width = 145
    Height = 25
    Caption = #25171#24320
    TabOrder = 7
    OnClick = Button1Click
  end
  object nrComm1: TnrComm
    Active = False
    BaudRate = 9600
    Parity = pNone
    StopBits = sbOne
    ByteSize = 8
    ComPortNo = 1
    ComPort = cpCOM1
    TraceStates = []
    EventChar = #0
    StreamProtocol = spNone
    BufferInSize = 4096
    BufferOutSize = 4096
    TimeoutRead = 0
    TimeoutWrite = 100
    RS485Mode = False
    EnumPorts = epFullPresent
    UseMainThread = True
    Terminal = Memo1
    TerminalUsage = tuBoth
    TerminalEcho = False
    Left = 400
    Top = 112
  end
end
