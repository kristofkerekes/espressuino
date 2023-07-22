var UI_VERSION = "1.4"

function openSideMenu(){
	if (document.getElementById('side-menu') != null) {
		document.getElementById('side-menu').style.width ='250px';
	}
	if (document.getElementById('main') != null) {
		document.getElementById('main').style.marginLeft ='250px';
	}
}

function closeSideMenu(){
	if (document.getElementById('side-menu') != null) {
		document.getElementById('side-menu').style.width ='0';
	}
	if (document.getElementById('main') != null) {
		document.getElementById('main').style.marginLeft ='0';
	}
}

function getMachineName (targetControl) {
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			targetControl.text(this.responseText);
		}
	};
	
	xhttp.open("GET", "/machine_name", true);
	xhttp.send();
}

function getPageTitle(){
	getMachineName($("#page-title"));
}

function getMainPageTitle(){
	getMachineName($("#main-page-title"));
}

function getMachineNameForInput(){
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			document.getElementById("machineName").value = this.responseText;
		}
	};
	
	xhttp.open("GET", "/machine_name", true);
	xhttp.send();
}

function saveParameters() {
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		document.getElementById('modal_dialog').style.display='block';
		modal_text = document.getElementById('modal-text');
		if (this.readyState == 4 && this.status == 200) {
			modal_text.innerHTML = this.responseText;
		} else {
			modal_text.innerHTML = "Failed to Save Parameters";
		}
	};
	
	xhttp.open("POST", "/save_parameters", true);	
	xhttp.send();
}

function getFirmwareVersion() {
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			$("#version").text(this.responseText);
		}
	};
	
	xhttp.open("GET", "/version", true);
	xhttp.send();
}

function getUIVersion() {
	$("#ui-version").text(UI_VERSION);
}

function getDeviceParameters() {
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			var responseJson = JSON.parse(this.responseText);
			document.getElementById("wifiSSID").value = responseJson.wifiSSID;
			document.getElementById("wifiPassword").value = responseJson.wifiPassword;
			document.getElementById("wifiMode").checked = !responseJson.wifiMode;
			document.getElementById("stationAddr").value = responseJson.stationAddr;
		}
	};
	
	xhttp.open("GET", "/device_parameters", true);
	xhttp.send();
}


function setDeviceParameters() {
	wifiSSID = document.getElementById("wifiSSID").value
	wifiPassword = document.getElementById("wifiPassword").value
	wifiAPMode = !document.getElementById("wifiMode").checked
	stationAddr = document.getElementById("stationAddr").value

	machineName = document.getElementById("machineName").value
	
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			$("#boiler-target-temperature").text(parseFloat(this.responseText));
		}
	};

	xhttp.open("POST", "/device_parameters", true);
	xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	
	var params = encodeURIComponent("wifiSSID") + "=" + encodeURIComponent(wifiSSID)
	     + "&" + encodeURIComponent("wifiPassword") + "=" + encodeURIComponent(wifiPassword)
		 + "&" + encodeURIComponent("wifiMode") + "=" + encodeURIComponent(wifiAPMode.toString ())	
		 + "&" + encodeURIComponent("stationAddr") + "=" + encodeURIComponent(stationAddr)	
		 + "&" + encodeURIComponent("machineName") + "=" + encodeURIComponent(machineName);	
	xhttp.send(params);
}


function setAndSaveDeviceParameters() {
	setDeviceParameters();
	saveParameters();
}

function changeParameterBy(delta, textControl, resourceName) {
	var currentValue = parseFloat(textControl.text());
	var floatDelta = parseFloat(delta);
	var newValue = currentValue + floatDelta;
	
	setRawParameter(resourceName, newValue, function (responseText) {
		textControl.text(parseFloat(responseText));
	});
}

function getRawParameter(resourceName) {
	var retValue = null;
	
	var xhttp = new XMLHttpRequest();
	
	xhttp.open("GET", resourceName, false);
	xhttp.send();
	
	if (xhttp.status == 200) {
		retValue = xhttp.responseText;
	}
		
	return retValue;
}

function setRawParameter(resourceName, newValue, onReady) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			onReady(this.responseText);
		}
	};

	xhttp.open("POST", resourceName, true);
	xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	
	var params = encodeURIComponent("value") + "=" + encodeURIComponent(newValue.toString());	
	xhttp.send(params);
}	

function getParameter(textControl, resourceName) {
	var xhttp = new XMLHttpRequest();
	
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			textControl.text(parseFloat(this.responseText));
		}
	};
	
	xhttp.open("GET", resourceName, true);
	xhttp.send();
}

function restartDevice ()
{
	var xhttp = new XMLHttpRequest();
	xhttp.open("POST", "/restart_device", true);
	xhttp.send();
}

function uploadFirmware() {
	var fileBrowser = document.getElementById('file');
	var modalDialog = document.getElementById('modal_dialog');
	var modalText = document.getElementById('modal-text');
	
	var isFirmware = fileBrowser.files[0].name.includes("_firmware");
	var isSpiffs = fileBrowser.files[0].name.includes("_spiffs");
	
	if (!isFirmware && !isSpiffs) {
		modalDialog.style.display='block';
		modalText.innerHTML = "Failed to upload Firmware! Unsupported file.";
	}
	
	var fileProgressBar = document.getElementById('file-progress');
	fileProgressBar.style.display='block';
	
	var selectedFile = fileBrowser.files[0];
	
	var data = new FormData();
	
	if (isFirmware) {
		data.append("file", selectedFile, "firmware");
	}
	if (isSpiffs) {
		data.append("file", selectedFile, "spiffs");
	}
	
    data.append("upload_file", true);
	
	$.ajax({
		url: '/update_firmware',
		type: 'POST',
		data: data,
		contentType: false,
		processData: false,
		xhr: function() {
			var xhr = new window.XMLHttpRequest();
			xhr.upload.onprogress = function (evt) {
				if (evt.lengthComputable) {
					var per = parseInt((evt.loaded / evt.total) * 100);
					fileProgressBar.value = per;
				}			
			};
			return xhr;
		},
		success:function(d, s) {
			modalDialog.style.display='block';
			modalText.innerHTML = "Firmware successfully uploaded! Restarting device...";
			restartDevice ();
		},
		error: function (a, b, c) {
			modalDialog.style.display='block';
			modalText.innerHTML = "Failed to upload Firmware!";
		}
	});
}