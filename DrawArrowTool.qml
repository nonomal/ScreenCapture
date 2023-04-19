import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 100
    visible: App.drawToolState === 2 && App.cutAreaState === 2
    DrawBtn {
        id: empty
        code: "\ue61a"
        isSelected: !App.rectCircle.isFill
        onClick: () => {
                     App.arrow.isFill = !App.rectCircle.isFill
                     App.arrow.bgColor = "transparent"
                     if (Shapes.shapes.length > 0 && Shapes.shapes[0].showDragger) {
                         Shapes.shapes[0].bgColor = "transparent"
                     }
                 }
    }
    DrawBtn {
        id: fill
        code: "\ue62c"
        anchors.left: empty.right
        anchors.top: empty.top
        isSelected: App.rectCircle.isFill
        onClick: () => {
                     App.arrow.isFill = !App.rectCircle.isFill
                     App.arrow.bgColor = "red"
                     if (Shapes.shapes.length > 0 && Shapes.shapes[0].showDragger) {
                         Shapes.shapes[0].bgColor = "red"
                     }
                 }
    }
    ColorPicker {
        anchors.left: fill.right
        anchors.top: fill.top
    }
}
