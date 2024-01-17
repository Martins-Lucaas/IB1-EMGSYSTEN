/**
 * @constructor
 */
function WebGLDetect(canvas) {
	this.browser = new BrowserDetect();
	var b = this.browser, glCanvas = null, 
		tempCanvas = $('<canvas/>').attr({
			"width": "0",
			"height": "0"
		});
	
	// Use provided canvas if possible
	if ($(canvas).is('canvas')) {
		glCanvas = $(canvas);
	} else {
		// Use temporary canvas
		glCanvas = tempCanvas;
		glCanvas.appendTo('body');
	}
	
	var gl = WebGLUtils.setupWebGL(glCanvas.get()[0], { 'antialias': true, 'stencil': true });
	this.detected = !!(gl);
	this.failID = null;
	this.systemFail = false;
	this.browserFail = false;
	
	if (!this.detected) {
		// WebGL not detected, try to find reason
		if (!this.isSupportedOS()) {
			this.failID = "os-not-supported";
			this.systemFail = true;
		} else if (!this.isSupportedBrowser()) {
			if (this.initChromeFrame()) {
				this.failID = "chrome-frame-prompt";
			} else {
				this.failID = "browser-not-supported";
			}
			this.browserFail = true;
		} else if (b.OS === "Mac" && b.OSversion === 10.7 && b.browser === "Chrome") {
			this.failID = "lion-chrome-not-supported";
			this.systemFail = true;
		} else {
			this.failID = "gl-bad-init";
			this.systemFail = true;
		}
	}
	
	// Remove temporary canvas
	tempCanvas.remove();
}

WebGLDetect.prototype = {
	trackResult: function() {
	},

	showGLFailPage: function() {
		// Show GL fail div.
		$("#gl-fail").css("display", "block");
	
		// Reuse logo for debug UI button
		$("#fail-topbar-logo").click(function() { window.location = "http://www.tinkercad.com"; });
		
		// Show correct fail message
		$("#"+this.failID).show();
	},

	initChromeFrame: function() {
		if (this.browser.browser === "Explorer" && window['CFInstall'] !== undefined && window['CFInstall']['isAvailable']() === false) {
			// Initialise Chrome Frame prompt
			$("#chrome-frame-install-but").click(function() {
				window['CFInstall']['check']({ 'mode': 'popup' });
				return false;
			});
			return true;
		} else {
			return false;
		}
	},
	
	// Whitelist supported browsers
	isSupportedBrowser: function() {
		if (this.browser.browser === "Chrome" && this.browser.version >= 10) {
			return true;
		}
		if (this.browser.browser === "Firefox" && this.browser.version >= 4) {
			return true;
		}
		return false;
	},
	
	// Whitelist supported OSes
	isSupportedOS: function() {
		if (this.browser.OS === "Mac" && this.browser.OSversion > 10.5) return true;
		if (this.browser.OS === "Windows" && this.browser.OSversion > 5.1) return true; // newer than Windows XP
		return false;
	}
};


/**
 * @constructor
 */
function BrowserDetect() {
	this.browser = this.searchString(this.dataBrowser) || "unknown browser";
	this.version = this.searchVersion(navigator.userAgent)
		|| this.searchVersion(navigator.appVersion)
		|| "";
	this.OS = this.searchString(this.dataOS) || "unknown OS";
	this.OSversion = this.searchVersion(navigator.userAgent)
		|| this.searchVersion(navigator.appVersion)
		|| "";
}

BrowserDetect.prototype = {
	
	searchString: function(data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1).replace(/_/g,'.'));
	},
	
	dataBrowser: [
		{
			string: navigator.userAgent,
			subString: "Chrome",
			identity: "Chrome"
		},
		{
			string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari",
			versionSearch: "Version"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows",
			versionSearch: "Windows NT"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac",
			versionSearch: "Mac OS X"
		},
		{
			string: navigator.userAgent,
			subString: "iPhone",
			identity: "iPhone/iPod"
		},
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]
	
};
