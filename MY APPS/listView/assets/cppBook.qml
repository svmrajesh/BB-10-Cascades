import bb.cascades 1.0
import "Common"

EbookPage {
    EbookContainer {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        topPadding: 10
        leftPadding: 10
        bottomPadding: 10
        rightPadding: 10
        Label {
            multiline: true
            text: "\t C++ Program:\n
            #include<iostream.h>
            #include<conio.h>
            void main()
            {
                cout<< 'hi'
            }
            		       
            
            "
        }

    }
}
