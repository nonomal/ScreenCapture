import QtQuick 2.15
import ScreenCapture.Cutter 1.0
import "CutArea.js" as CutArea
import "MouseTip.js" as MouseTip

MouseArea {
    id: root
    property int mouseInArea: -1 //0左上,1上,2右上,3右,4右下,5下,6左下,7左,8中
    property int mousePressX: 0
    property int mousePressY: 0
    anchors.fill: parent
    visible: true //todo 绘图状态时要控制这里不显示
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    cursorShape: Qt.CrossCursor
    onPositionChanged: mouse => {
                           if (App.cutAreaState === 0) {
                               if (mouse.buttons === Qt.NoButton) {
                                   //高亮窗口区域
                                   Cutter.createCutAreaByWindowRect()
                               }
                           } else if (App.cutAreaState === 1) {
                               //todo 出现大小控制工具
                               if (mouse.buttons === Qt.LeftButton) {
                                   CutArea.createCutAreaByDrag(mouse)
                               }
                           } else if (App.cutAreaState === 2) {
                               //设置Cursor
                               if (mouse.buttons === Qt.NoButton) {
                                   CutArea.setCursorByMousePosition(mouse)
                               }
                           } else if (App.cutAreaState === 3) {
                               //改变大小
                               if (mouse.buttons === Qt.LeftButton) {
                                   CutArea.resizeCutArea(mouse)
                               }
                           } else if (App.cutAreaState === 4) {
                               if (mouse.buttons === Qt.LeftButton) {
                                   //移动截图区域
                                   CutArea.moveCutAreaByDrag(mouse)
                               }
                           }

                           //放大镜
                           MouseTip.setMouseTip(mouse)
                       }
    onPressed: mouse => {
                   if (mouse.buttons === Qt.LeftButton) {
                       root.mousePressX = mouse.x
                       root.mousePressY = mouse.y
                       if (App.cutAreaState === 0) {
                           App.cutAreaState = 1
                       } else if (App.cutAreaState === 2) {
                           if (mouseInArea != 8 && mouseInArea != -1) {
                               //改变大小
                               App.cutAreaState = 3
                               CutArea.resizeCutArea(mouse)
                           } else {
                               //移动位置
                               App.cutAreaState = 4
                           }
                       } else if (App.cutAreaState === 4) {

                           //开始绘图，也有可能要拖动改变选区
                       }
                   }
               }
    onReleased: mouse => {
                    if (mouse.button === Qt.LeftButton) {
                        if (App.cutAreaState === 3) {
                            App.cutAreaState = 2
                            CutArea.updatePoint()
                        } else if (App.cutAreaState === 1 || App.cutAreaState === 4) {
                            App.cutAreaState = 2
                        }
                    }
                }
}