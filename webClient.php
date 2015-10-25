<?php

	if($_SERVER["REQUEST_METHOD"] === "POST") {
		//Decode client data
		$json = json_decode(file_get_contents("php://input"));
		$out = count($json->axes) . "," . count($json->buttons) . "," . implode(",", $json->axes) . "," .  implode(",", $json->buttons) . "\n";

		//Open file
		$file = fopen("pipe", "w");
		if($file) {
			//Send data to telepresence client pipe
			fwrite($file, $out);
			fclose($file);
		} else {
			echo $errstr . " (" . $errno . ")\n";
		}
		die();
	}

?>

<html>

<head>
	<title>Telepresence Web Client</title>
	<style>
		@import url(http://fonts.googleapis.com/css?family=Roboto);

		body {
			color:#DDDDDD;
			background-color:#222222;
			font-family:"Roboto", "Arial", sans-serif;
		}

		.joystick {
			float:left;
			padding:10px;
			/*background-color:#000000;*/
		}

		.buttons {
			/*background-color:#000000;*/
		}

		.upArrow {
			width: 0;
			height: 0;
			border-left: 50px solid transparent;
			border-right: 50px solid transparent;
			border-bottom: 100px solid darkgreen;
			cursor:pointer;
		}

		.leftArrow {
			width: 0;
			height: 0;
			border-top: 50px solid transparent;
			border-right: 100px solid darkgreen;
			border-bottom: 50px solid transparent;
			cursor:pointer;
		}

		.rightArrow {
			width: 0;
			height: 0;
			border-top: 50px solid transparent;
			border-left: 100px solid darkgreen;
			border-bottom: 50px solid transparent;
			cursor:pointer;
		}

		.downArrow {
			width: 0;
			height: 0;
			border-top: 100px solid darkgreen;
			border-left: 50px solid transparent;
			border-right: 50px solid transparent;
			cursor:pointer;
		}

		.arrowBox {
			width:50px;
			height:50px;
			cursor:pointer;
		}

		.level3 {
			background-color:green;
		}

		.level2 {
			background-color:yellow;
		}

		.level1 {
			background-color:orange;
		}

		.stop {
			font-size:35px;
			font-weight:bold;
			text-align:center;
			color:black;
			background-color:red;
		}

		.on {
			width:50px;
			font-weight:bold;
			background-color:green;
		}

		.off {
			width:50px;
			font-weight:bold;
			background-color:red;
		}
	</style>
	<script type="text/javascript">
		var JOY_ALL = -1;
		var JOY_X = 0;
		var JOY_Y = 1;

		var axes = [0.0, 0.0];
		var buttons = ["F", "F", "F", "F","'F", "F", "F", "F", "F", "F"];

		function joystick(axis, value) {
			if(axis == JOY_ALL) {
				axes[JOY_X] = value;
				axes[JOY_Y] = value;
			} else {
				axes[axis] = value;
			}
			serialize();
		}

		function button(num) {
			if(buttons[num] === "F") {
				buttons[num] = "T";
				document.getElementById("button" + num).value = "ON";
				document.getElementById("button" + num).className = "on";
			} else {
				buttons[num] = "F";
				document.getElementById("button" + num).value = "OFF";
				document.getElementById("button" + num).className = "off";
			}
			serialize();
		}

		function serialize() {
			var postdata = {
				axes: axes,
				buttons: buttons
			};
			document.getElementById("output").innerHTML = JSON.stringify(postdata);
			var xmlhttp = new XMLHttpRequest();
			xmlhttp.open("POST", window.location.pathname, true);
			xmlhttp.send(JSON.stringify(postdata));
		}
	</script>
</head>

<body>
	<h2>Telepresence Web Client</h2>

	<table class="joystick">
		<tr>
			<td colspan="4"></td>
			<td class="upArrow level4" onclick="joystick(JOY_Y, 1.0);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level3" onclick="joystick(JOY_Y, 0.75);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level2" onclick="joystick(JOY_Y, 0.5);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level1" onclick="joystick(JOY_Y, 0.25);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td class="leftArrow level4" onclick="joystick(JOY_X, -1.0);"></td>
			<td class="arrowBox level3" onclick="joystick(JOY_X, -0.75);"></td>
			<td class="arrowBox level2" onclick="joystick(JOY_X, -0.5);"></td>
			<td class="arrowBox level1" onclick="joystick(JOY_X, -0.25);"></td>
			<td class="arrowBox stop" onclick="joystick(JOY_ALL, 0.0);">STOP</td>
			<td class="arrowBox level1" onclick="joystick(JOY_X, 0.25);"></td>
			<td class="arrowBox level2" onclick="joystick(JOY_X, 0.5);"></td>
			<td class="arrowBox level3" onclick="joystick(JOY_X, 0.75);"></td>
			<td class="rightArrow level4" onclick="joystick(JOY_X, 1.0);"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level1" onclick="joystick(JOY_Y, -0.25);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level2" onclick="joystick(JOY_Y, -0.5);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="arrowBox level3" onclick="joystick(JOY_Y, -0.75);"></td>
			<td colspan="4"></td>
		</tr>
		<tr>
			<td colspan="4"></td>
			<td class="downArrow level4" onclick="joystick(JOY_Y, -1.0);"></td>
			<td colspan="4"></td>
		</tr>
	</table>

	<div class="buttons">
		Button 0: <input id="button0" type="button" value="OFF" class="off" onclick="button(0);" /><br />
		Button 1: <input id="button1" type="button" value="OFF" class="off" onclick="button(1);" /><br />
		Button 2: <input id="button2" type="button" value="OFF" class="off" onclick="button(2);" /><br />
		Button 3: <input id="button3" type="button" value="OFF" class="off" onclick="button(3);" /><br />
		Button 4: <input id="button4" type="button" value="OFF" class="off" onclick="button(4);" /><br />
		Button 5: <input id="button5" type="button" value="OFF" class="off" onclick="button(5);" /><br />
		Button 6: <input id="button6" type="button" value="OFF" class="off" onclick="button(6);" /><br />
		Button 7: <input id="button7" type="button" value="OFF" class="off" onclick="button(7);" /><br />
		Button 8: <input id="button8" type="button" value="OFF" class="off" onclick="button(8);" /><br />
		Button 9: <input id="button9" type="button" value="OFF" class="off" onclick="button(9);" /><br />
	</div>

	<br />
	Output:
	<div id="output" onclick="serialize();">
		No Data Sent
	</div>
</body>

</html>
