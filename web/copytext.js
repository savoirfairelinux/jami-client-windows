"use strict"

function copy_text_selected() {
    var selObj = document.getSelection();
    var selectedText = selObj.toString();
    return selectedText;
}
