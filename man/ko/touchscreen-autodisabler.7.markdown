# touchscreen-autodisabler(7) - 자동으로 터치스크린을 켜고 꺼요.

## 사용법

touchscreen-autodisabler  
touchscreen-autodisabler {-h, -v}

systemctl --user {start, stop, ...} touchscreen-autodisabler[.service]

## 설명

타블렛 스타일러스 펜과 지우개의 근접 상태에 따라 터치스크린을 켜고 꺼요.

### 작동 방식

이 프로그램은 X 서버로부터 타블렛 이벤트를 받아요. 펜이 타블렛에 가까이 오면,
X 서버는 펜의 상태가 바뀐 이벤트를 보내요. 이 이벤트가 감지되면, 터치스크린을
잡아서 터치 이벤트가 다른 X 클라이언트로 전달 되지 않게 해요.

펜이 빠지면 터치스크린을 놓아주어, 다시 작동할 수 있게 해요.

### 단일 실행

이 프로그램은 **단일 실행**되는 서비스와 비슷한 프로그램이 되도록 만들어 졌어요.
이 프로그램이 작동하면, DBus에서 **"wsid.Tad"**라는 이름을 가져가요. 그리고
이 프로그램에 대한 [메소드, 그리고 속성을 그곳][DBUS 인터페이스]에 노출시켜요.

이 프로그램과 함께 systemd 사용자 서비스 파일이 설치되요. systemctl(1)로
시작하거나, 끌 수 있고, 또한 시작시 자동 실행 되도록 할 수 있어요.

        systemctl --user start touchscreen-autodisabler
        # touchscreen autodisabler를 시작해요.

        systemctl --user enable touchscreen-autodisabler
        # touchscreen autodisabler를 자동으로 시작하도록 해요.

## 옵션

GSettings와 DBus를 써서 명령행 옵션은 별로 없네요.

 * -h, --help:
     touchscreen autodisabler의 명령행 옵션에 대해 간단한 도움말을 출력해요.

 * -v, --version:
     touchscreen autodisabler의 버전을 출력해요.

## GSETTINGS

이 프로그램은 GSettings에 옵션을 저장해요. gsettings(1)이나 dconf-editor(1)를
통해 옵션을 설정할 수 있어요. 장치 이름에 대해서는 장치의 ID와 장치 이름 둘다
쓸 수 있어요. 장치 목록이 비어있으면, 이에 해당하는 장치들을 얻으려고 해요.

### wsid.Tad 스키마 @ /wsid/Tad

 * watch: **as**:
    감시되는 장치들
    근접 상태 변경을 알려줄 타블렛 스타일러스나 지우개입니다.

 * control: **as**:
    제어되는 장치들
    활성화 그리고 비활성화 될 터치스크린입니다.

 * transition: **s** = {"default", "clear-to-enable"}:
    상태 전환 모드
    내부 상태가 어떻게 전환될지, 그리고 장치를 어떻게 활성화 그리고 비활성화 할지
    설정해요.

    "default" : 간단한 상태 전환이에요. 들어오면 비활성화, 나가면 활성화해요.

    "clear-to-enable": 터치를 모두 뗄때까지 터치스크린 활성을 멈춰요. 펜이 나간
    이후에도 손을 계속 기댈 수 있는데, 이때 의도치 않은 터치 이벤트를 막아줘요.

## DBUS 인터페이스

이 프로그램은 상태와 메소드를 DBus를 통해 노출하고 있어요.

### 인터페이스 wsid.Tad @ /wsid/Tad 객체

 * _속성_ State: **u** (read):
   응용 프로그램 상태에요. 값은 다음 의미를 가지고 있어요.  
   0: 중립 상태  
   1: 펜 상태  
   2: 펜 기댐 상태 (손이 터치스크린에 기대고 있어요.)  
   3: 여전히 기댐 상태. (펜은 나갔지만 손은 여전히 기대고 있어요.)  
   4: 터치 상태. (터치스크린 사용중이에요.)

 * _속성_ ControlEnabled: **b** (read):
   제어되는 장치들이 활성화 되어있는지 아닌지입니다.

 * _메소드_ ResetState (): () => ():
   상태를 중립으로 되돌립니다.

 * _메소드_ Quit (): () => ():
   touchscreen autodisabler를 끝냅니다.

 * _신호_ Transit: => (u, u, b):
               stimulus: u: 이벤트에요.  
               state: u: 새로운 상태에요.  
               control-enabled: b: 제어되는 장치들이 활성화 되어야 하는지에요.  
    상태가 바뀔때마다 방출되는 신호에요.
       
## 관련 항목

xinput (1)

## 저작권

2014-2016, 저작권은 WSID에게 있습니다.

## 작성자

WSID <jongsome@gmail.com> <http://wsidre.egloos.com>
