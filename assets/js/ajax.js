function getXmlHttpRequest()
{
    var xmlRequest=null;
    if (window.XMLHttpRequest) {
        //如果是高版本 浏览器
        xmlRequest = new XMLHttpRequest();//创建对象
    } else {
        //低版本
        xmlRequest = new ActiveXObject("Microsoft.XMLHTTP");
    }
    return xmlRequest;
}


function ajax_deal() {
    //1、创建XMLHttpRequest对象
    var xmlRequest = null;
    xmlRequest = getXmlHttpRequest();

    //2、与回调函数关联起来
    xmlRequest.onreadystatechange=function()
    {
        document.getElementById("label").innerHTML=xmlRequest.responseText;
        // alert(xmlRequest.readyState);
        // alert(xmlRequest.status);
        if(xmlRequest.readyState==4 && xmlRequest.status==200)
        {
            //用户要实现的地方
            //获取服务器的应答  而服务器应答存放在xmlRequest.responseText里面
            document.getElementById('label').innerHTML = xmlRequest.responseText;
        }
    }

    //3、使用open方法 创建请求
    xmlRequest.open("GET", "test.txt", true);
    xmlRequest.setRequestHeader("If-Modified-Since", "0");

    //4、发送请求
    xmlRequest.send();
}
