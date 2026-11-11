import QtQuick
Rectangle {
 color: "#151d2b"
 function onActivate() {}
 function onLeave() {}
 Column {
  anchors.centerIn: parent
  spacing: 28
  Text { text: "Um novo começo."; color: "#e6b85c"; font.pixelSize: 32 }
  Text { text: "Seus arquivos. Suas ideias. Seu espaço.\nO Dell OS está sendo instalado."; color: "#edf2fa"; font.pixelSize: 18; lineHeight: 1.6 }
 }
}
