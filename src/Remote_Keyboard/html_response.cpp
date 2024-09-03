
const char* root_body_script = R"rawliteral(
  <script>
    const socket = new WebSocket('ws://' + location.host + '/ws');
    const modifierKeys = { Shift: false, Control: false, Alt: false, Meta: false };  // 修飾キーの状態を追跡

    socket.onopen = function() {
      console.log('WebSocket connection established');
    };

    // テキストエリアの要素を取得
    const textarea = document.getElementById('keyCapture');

    // WiFi接続用の関数を追加
    function setWiFi() {
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;
      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/set', true);
      xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
          console.log('Response received: ' + xhr.responseText);
          document.body.innerHTML = xhr.responseText;  // 受け取ったレスポンスをページに反映
        } else if (xhr.readyState === 4) {
          console.error('Error: ' + xhr.status + ' ' + xhr.statusText);
        }
      };
      xhr.send('ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password));
      console.log('Request sent to /connect');
    }

    textarea.addEventListener('keydown', function(event) {
      const key = event.key;  // 実際のキー入力（文字）を取得
      const code = event.code.toLowerCase();  // 小文字で物理キーコードを取得

      // Tabキーのデフォルト動作を防止する処理
      if (key === 'Tab') {
        event.preventDefault();  // フォーカスが他の要素に移動するのを防ぐ
        socket.send('tab');  // TabキーをWebSocketで送信
        return;  // Tabキーの処理が終わったので、ここで終了
      }

      // 修飾キーの押下を管理
      if (modifierKeys.hasOwnProperty(key)) {
        if (!modifierKeys[key]) {  // 修飾キーがまだ押されていない場合
          modifierKeys[key] = true;  // 修飾キーが押された状態を保持
        }
        return;  // 修飾キーの処理はここで終了
      }

      // 特殊なキーリストを定義（すべて小文字）
      const specialKeys = [
        'f1', 'f2', 'f3', 'f4', 'f5', 'f6', 'f9', 'f10', 'f11', 'f12',
        'home', 'end', 'insert', 'delete', 'pageup', 'pagedown',
        'scrolllock', 'arrowup', 'arrowright', 'arrowdown', 'arrowleft',
        'capslock', 'tab', 'numlock', 'space', 'enter'
      ];

      // 特殊キーかどうかを判定し、通常のキーか特殊キーかで処理を分ける
      let keyToSend;
      const activeModifiers = Object.keys(modifierKeys).filter(mod => modifierKeys[mod]).join('+');

      if (specialKeys.includes(code)) {
        // 特殊キーの場合、修飾キーを含めて送信
        keyToSend = activeModifiers ? `${activeModifiers}+${code}` : code;  // Shiftキーも含める
      } else {
        // 通常のキーの場合でも、Shiftキー以外の修飾キーがある場合はその組み合わせを送信
        if (activeModifiers && activeModifiers !== 'Shift') {
          keyToSend = `${activeModifiers}+${key}`;  // ControlキーやAltキーの組み合わせを考慮
        } else {
          // Shiftキーの影響を無視して通常のキーを送信
          keyToSend = key;
        }
      }

      socket.send(keyToSend);  // 組み合わせたキーをWebSocketで送信
    });

    textarea.addEventListener('keyup', function(event) {
      const key = event.key;
      if (modifierKeys.hasOwnProperty(key)) {
        modifierKeys[key] = false;  // 修飾キーが離されたら状態をリセット
      }
    });
  </script>
)rawliteral";

const char* root_html_header = R"rawliteral(
  <head>
    <title>Remote USB Keyboard by M5StampS3</title>
    <style>
      /* フォーム全体のスタイル */
      #wifiForm {
        display: flex; /* 横並びにする */
        flex-wrap: wrap; /* 狭い画面で折り返す */
        align-items: center; /* 垂直方向の中央揃え */
        gap: 10px; /* 各要素の間に間隔をあける */
        margin-bottom: 20px; /* 下に余白を追加 */
      }
      /* 説明文のスタイル */
      #description {
        margin-bottom: 20px; /* 下に余白を追加 */
      }
      /* 各入力フィールドのスタイル */
      #wifiForm input[type="text"],
      #wifiForm input[type="password"] {
        padding: 5px; /* 入力フィールドの内側に余白を追加 */
        flex: 1; /* フレックスアイテムとして伸縮 */
        min-width: 150px; /* 最小幅を指定 */
      }
      /* 接続ボタンのスタイル */
      #wifiForm button {
        padding: 5px 10px; /* ボタンの内側に余白を追加 */
        flex-shrink: 0; /* ボタンのサイズを縮めない */
      }
      /* スマホなどの小さい画面でのスタイル調整 */
      @media (max-width: 600px) {
        #wifiForm {
          flex-direction: column; /* 縦並びに変更 */
          align-items: stretch; /* 要素を伸ばす */
        }
        #wifiForm input[type="text"],
        #wifiForm input[type="password"],
        #wifiForm button {
          width: 100%; /* 幅を100%に設定 */
          max-width: 100%; /* 最大幅を制限 */
        }
      }
    </style>
  </head>
)rawliteral";
