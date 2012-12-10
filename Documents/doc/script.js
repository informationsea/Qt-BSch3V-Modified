function getHeadline()
{
    var h2s = document.getElementsByTagName("h2");
    //alert(h2s.length);
    var text = "<h2>目次</h2><ul>";
    var x = 0;
    
    for(x = 0;x < h2s.length;x++){
	text += "<li><a href=\"#headline"+x+"\">"+h2s[x].innerHTML+"</a></li>";
	h2s[x].innerHTML = "<a name=\"headline"+x+"\"></a>" + h2s[x].innerHTML;
    }
    
    text += "</ul>";
    return text;
}

this.onload = load;

function load()
{
    var headline = document.getElementById("headline");
    if(headline){
	headline.innerHTML = getHeadline();
    }
}