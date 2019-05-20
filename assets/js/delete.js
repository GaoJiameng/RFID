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



function on_button_del()
{
    var info = "";
    var rfid_del = document.getElementById("rfid_del").value.toLocaleLowerCase();

    if("" == rfid_del)
    {
        alert("请刷卡,然后点击确定!");
        info += "$$$";
    }
    else
    {
        info += rfid_del;
    }

    //创建ajax对象
    var xmlRequest=null;
    xmlRequest = getXmlHttpRequest();

    //关联回调函数
    xmlRequest.onreadystatechange=function()
    {
        if(xmlRequest.readyState==4 && xmlRequest.status==200)
        {
            //用户要实现的地方
            //获取服务器的应答  而服务器应答存放在xmlRequest.responseText里面
            var text = xmlRequest.responseText;
            var arr = text.split("-");
            // alert(arr[0]);

            if(arr[0] == "1") //删除成功
            {
                // alert(arr[1]);
                alert("删除成功");
            }
            else if(arr[0] == "0")  //删除失败
            {
                // alert(arr[1]);
                alert("删除失败");
            }

        }
    }

    var URL = "/cgi-bin/delete.cgi?";
    xmlRequest.open("POST",URL,true);
    xmlRequest.send(info);
    // alert(info);
}