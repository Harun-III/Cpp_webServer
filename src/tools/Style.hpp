#ifndef STYLE_HPP
# define STYLE_HPP

# define STYLE \
    "<style>" \
    "@import url('https://fonts.googleapis.com/css2?family=Fira+Code:wght@300;400;500;700&display=swap');" \
    \
    "* {margin:0; padding:0; box-sizing:border-box}" \
    \
    "body {" \
        "background:linear-gradient(135deg,#0a0a0a,#1a1a1a 25%,#0d1117 75%,#000);" \
        "color:#00ff41; font-family:'Fira Code',monospace; min-height:100vh;" \
        "display:flex; align-items:center; justify-content:center" \
    "}" \
    \
    ".c {max-width:1000px; margin:0 auto; padding:24px}" \
    "h1 {margin:0 0 8px; font-weight:700; text-shadow:0 0 12px #00ff41}" \
    "hr {" \
        "border:0; height:1px; margin:10px 0 16px;" \
        "background:linear-gradient(90deg,transparent,rgba(0,255,65,.3) 10%,rgba(0,255,65,.3) 90%,transparent)" \
    "}" \
    \
    ".e {text-align:center}" \
    ".e h1 {font-size:8rem; line-height:1; margin:0}" \
    ".e hr {" \
        "width:80%; max-width:400px; margin:24px auto 20px;" \
        "background:linear-gradient(90deg,transparent,rgba(0,255,65,.5) 20%,rgba(0,255,65,.5) 80%,transparent)" \
    "}" \
    ".e p {font-size:1.5rem; margin-top:10px; font-weight:500; opacity:.9}" \
    \
    ".f {position:fixed; bottom:20px; left:50%; transform:translateX(-50%); font-size:.9rem; opacity:.5}" \
    \
    ".l {display:grid; gap:12px}" \
    ".i {display:flex; align-items:center; gap:8px}" \
    ".i a {" \
        "flex:1; color:#00ff41; text-decoration:none; border:1px solid #00ff41;" \
        "padding:6px 12px; border-radius:6px; text-align:left;" \
        "background:linear-gradient(45deg,rgba(0,255,65,.08),rgba(0,255,65,.18));" \
        "font-weight:500; letter-spacing:1px; transition:all .3s ease;" \
        "white-space:nowrap; overflow:hidden; text-overflow:ellipsis" \
    "}" \
    ".i a:hover {background:#00ff41; color:#000; box-shadow:0 0 14px rgba(0,255,65,.5)}" \
    \
    ".d {" \
        "background:none; border:1px solid #ff4136; color:#ff4136;" \
        "padding:6px 12px; border-radius:6px; min-width:60px; cursor:pointer;" \
        "font-family:'Fira Code',monospace; font-weight:500; letter-spacing:1px;" \
        "transition:all .3s ease; font-size:0.85rem" \
    "}" \
    ".d:hover {background:#ff4136; color:#000; box-shadow:0 0 14px rgba(255,65,54,.5)}" \
    ".d:disabled {opacity:0.3; cursor:not-allowed}" \
    \
    ".parent {display:block; width:100%}" \
    \
    ".modal {" \
        "display:none; position:fixed; top:0; left:0; width:100%; height:100%;" \
        "background:rgba(0,0,0,0.85); z-index:1000;" \
        "align-items:center; justify-content:center" \
    "}" \
    ".modal.show {display:flex}" \
    ".modal-content {" \
        "background:linear-gradient(135deg,#1a1a1a,#0d1117);" \
        "border:2px solid #00ff41; border-radius:12px; padding:30px;" \
        "max-width:450px; width:90%; animation:slideIn 0.3s ease;" \
        "box-shadow:0 0 30px rgba(0,255,65,0.3)" \
    "}" \
    \
    "@keyframes slideIn {from{transform:translateY(-50px);opacity:0} to{transform:translateY(0);opacity:1}}" \
    \
    ".modal-title {" \
        "font-size:1.5rem; margin-bottom:10px; font-weight:700;" \
        "color:#ff4136; text-shadow:0 0 10px rgba(255,65,54,0.5)" \
    "}" \
    ".modal-file {font-size:1.1rem; margin-bottom:20px; color:#00ff41; word-break:break-all; font-weight:500}" \
    ".modal-warning {" \
        "display:flex; align-items:center; gap:10px; margin:20px 0; padding:15px;" \
        "background:rgba(255,65,54,0.1); border:1px solid #ff4136; border-radius:6px" \
    "}" \
    ".modal-warning input[type=checkbox] {width:20px; height:20px; cursor:pointer; accent-color:#ff4136}" \
    ".modal-warning label {cursor:pointer; color:#ff4136; font-size:0.95rem; flex:1}" \
    \
    ".modal-buttons {display:flex; gap:12px; margin-top:25px}" \
    ".modal-btn {" \
        "flex:1; padding:12px; border:none; border-radius:6px; cursor:pointer;" \
        "font-family:'Fira Code',monospace; font-weight:500; letter-spacing:1px;" \
        "transition:all 0.3s ease; font-size:0.95rem" \
    "}" \
    ".modal-btn-cancel {background:#555; color:#fff; border:1px solid #777}" \
    ".modal-btn-cancel:hover {background:#666; box-shadow:0 0 10px rgba(255,255,255,0.2)}" \
    ".modal-btn-delete {background:none; border:1px solid #ff4136; color:#ff4136}" \
    ".modal-btn-delete:hover:not(:disabled) {background:#ff4136; color:#000; box-shadow:0 0 14px rgba(255,65,54,0.5)}" \
    ".modal-btn-delete:disabled {opacity:0.3; cursor:not-allowed}" \
    \
    ".error-popup {" \
        "position:fixed; top:20px; right:20px; display:none; z-index:2000;" \
        "background:linear-gradient(135deg,#1a1a1a,#0d1117);" \
        "border:2px solid #ff4136; border-radius:8px; padding:20px 25px;" \
        "min-width:300px; max-width:400px; animation:slideInRight 0.4s ease;" \
        "box-shadow:0 0 30px rgba(255,65,54,0.5)" \
    "}" \
    ".error-popup.show {display:block}" \
    ".error-popup.hide {animation:slideOutRight 0.4s ease}" \
    \
    "@keyframes slideInRight {from{transform:translateX(450px);opacity:0} to{transform:translateX(0);opacity:1}}" \
    "@keyframes slideOutRight {from{transform:translateX(0);opacity:1} to{transform:translateX(450px);opacity:0}}" \
    \
    ".error-popup-header {display:flex; align-items:center; gap:10px; margin-bottom:12px}" \
    ".error-popup-icon {font-size:1.8rem; line-height:1}" \
    ".error-popup-title {" \
        "font-size:1.2rem; font-weight:700; color:#ff4136;" \
        "text-shadow:0 0 8px rgba(255,65,54,0.5)" \
    "}" \
    ".error-popup-message {color:#00ff41; font-size:0.95rem; line-height:1.5; margin-bottom:15px}" \
    ".error-popup-code {" \
        "background:rgba(255,65,54,0.1); border:1px solid #ff4136; border-radius:4px;" \
        "padding:8px 12px; color:#ff4136; font-weight:500; font-size:0.9rem;" \
        "margin-bottom:15px; text-align:center" \
    "}" \
    ".error-popup-close {" \
        "background:#ff4136; color:#000; border:none; padding:8px 20px; border-radius:6px;" \
        "cursor:pointer; font-family:'Fira Code',monospace; font-weight:500; width:100%;" \
        "transition:all 0.3s ease; letter-spacing:1px" \
    "}" \
    ".error-popup-close:hover {box-shadow:0 0 14px rgba(255,65,54,0.7); transform:translateY(-2px)}" \
    "</style>"

# define DELETE_SCRIPT \
    "<script>" \
    "let delPath,delBtn;" \
    "const $=(id)=>document.getElementById(id);" \
    "const show=(el)=>el.classList.add('show');" \
    "const hide=(el)=>el.classList.remove('show');" \
    \
    "function showModal(path,btn){" \
        "delPath=path;" \
        "delBtn=btn;" \
        "show($('modal'));" \
        "$('fileName').textContent=path;" \
        "$('confirmCheck').checked=false;" \
        "$('deleteBtn').disabled=true" \
    "}" \
    \
    "function hideModal(){hide($('modal'))}" \
    \
    "function toggleDelete(){" \
        "$('deleteBtn').disabled=!$('confirmCheck').checked" \
    "}" \
    \
    "function showError(title,msg,code){" \
        "const popup=$('errorPopup');" \
        "$('errorTitle').textContent=title;" \
        "$('errorMessage').textContent=msg;" \
        "$('errorCode').textContent=code;" \
        "popup.classList.remove('hide');" \
        "show(popup);" \
        "setTimeout(hideError,5000)" \
    "}" \
    \
    "function hideError(){" \
        "const popup=$('errorPopup');" \
        "popup.classList.add('hide');" \
        "setTimeout(()=>popup.classList.remove('show','hide'),400)" \
    "}" \
    \
    "const errors={" \
        "500:['Internal Server Error','The server encountered an error while processing your request.']," \
        "403:['Permission Denied','You do not have permission to delete this file.']," \
        "404:['File Not Found','The file you are trying to delete does not exist.']" \
    "};" \
    \
    "function resetBtn(){" \
        "delBtn.disabled=false;" \
        "delBtn.textContent='DEL'" \
    "}" \
    \
    "function confirmDelete(){" \
        "hideModal();" \
        "delBtn.disabled=true;" \
        "delBtn.textContent='...';" \
        "fetch(delPath,{method:'DELETE'})" \
        ".then(r=>{" \
            "if(r.ok){" \
                "delBtn.parentElement.style.opacity='0.3';" \
                "setTimeout(()=>location.reload(),500)" \
            "}else{" \
                "const err=errors[r.status]||['Delete Failed','HTTP '+r.status];" \
                "showError(err[0],err[1],'Error '+r.status);" \
                "resetBtn()" \
            "}" \
        "})" \
        ".catch(()=>{" \
            "showError('Network Error','Failed to connect to the server.','Connection Failed');" \
            "resetBtn()" \
        "})" \
    "}" \
    "</script>" \
    "<div id=modal class=modal onclick=\"if(event.target===this)hideModal()\">" \
        "<div class=modal-content>" \
            "<div class=modal-title>⚠️ Delete File</div>" \
            "<div class=modal-file id=fileName></div>" \
            "<div class=modal-warning>" \
                "<input type=checkbox id=confirmCheck onchange=toggleDelete()>" \
                "<label for=confirmCheck>I understand this file will be permanently lost and cannot be recovered</label>" \
            "</div>" \
            "<div class=modal-buttons>" \
                "<button class=\"modal-btn modal-btn-cancel\" onclick=hideModal()>Cancel</button>" \
                "<button class=\"modal-btn modal-btn-delete\" id=deleteBtn onclick=confirmDelete() disabled>Delete</button>" \
            "</div>" \
        "</div>" \
    "</div>" \
    "<div id=errorPopup class=error-popup>" \
        "<div class=error-popup-header>" \
            "<div class=error-popup-icon>🚨</div>" \
            "<div class=error-popup-title id=errorTitle>Error</div>" \
        "</div>" \
        "<div class=error-popup-message id=errorMessage>An error occurred</div>" \
        "<div class=error-popup-code id=errorCode>Error 500</div>" \
        "<button class=error-popup-close onclick=hideError()>OK</button>" \
    "</div>"

#endif
