Hamdiz Truly Random Multi-ID Generator (MIG) (ESP32 Version)

Welcome to Hamdiz Truly Random Multi-ID Generator (MIG), a versatile and robust ID generation tool for the ESP32 platform. This project is designed to generate various types of unique IDs with advanced features such as streaming, customizable output destinations, and peripheral validation.

⚙️ Prerequisites

Before using the Hamdiz Multi-ID Generator, ensure you have installed all necessary dependencies and configured your environment correctly.

Required Libraries:

	1.	Wire.h: For I2C communication (used for LCD validation or other I2C peripherals).
	2.	SD.h: For SD card operations.
	3.	SPI.h: Required for interfacing with the SD card module.
	4.	WiFi.h: For WiFi connectivity (used for API integration).
	5.	HTTPClient.h: For sending HTTP requests to an API.
	6.	EEPROM.h: For saving and retrieving setup data persistently.

Baud Rate Configuration:

	•	Set the Serial Monitor to 921600 baud to ensure proper communication with the ESP32.

Installation Instructions:

	1.	Open the Arduino IDE.
	2.	Navigate to Sketch > Include Library > Manage Libraries.
	3.	Search for and install the required libraries listed above if not already installed.
	4.	Configure your Serial Monitor’s baud rate to 921600.

	⚠️ Failing to set up these dependencies or configure the baud rate may result in the project not functioning as expected.

🌟 Features

Core Features:

	•	Generate Unique IDs:
	•	Random Integer
	•	UUID v1 (timestamp-based)
	•	UUID v4 (random)
	•	Hexadecimal
	•	Base64
	•	Alphanumeric
	•	Binary
	•	Custom Character Sets
	•	Stream Mode: Continuously generates IDs at a specified interval.
	•	Setup Configuration: On first use, configure the output destination (Serial, File, or API). Modify settings anytime using the setup command.
	•	Save and Load Data: Write and retrieve data from an SD card for enhanced data management.

Advanced Features:

	1.	Peripheral Validation:
	•	Automatically checks for connected peripherals (e.g., SD card, LCD).
	•	Prints warnings if a required device is unavailable, ensuring smooth operation.
	2.	Customizable Output:
	•	Flexible output options:
	•	Serial: Outputs directly to the Serial Monitor.
	•	File: Saves generated IDs to an SD card file.
	•	API: Sends generated IDs to a specified API endpoint with optional authentication.
	3.	API Integration:
	•	Supports Bearer and Basic authentication.
	•	Configurable API endpoints and payloads.
	4.	Synonyms and Aliases:
	•	Commands support multiple aliases for intuitive use (e.g., uuid v1 or uuid1).
	5.	Error Handling:
	•	Comprehensive and user-friendly error messages for invalid inputs or missing peripherals.

📝 Commands

ID Generation:

	•	int <length> (or integer <length>): Generate a random integer with the specified length.
	•	uuid v1 (or uuid1 <length>) (or u1 <length>): Generate a UUID Version 1 (timestamp-based).
	•	uuid v4 (or uuid4 <length>) (or u4 <length>): Generate a UUID Version 4 (random).
	•	hex <length> (or hexadecimal <length>): Generate a random hexadecimal string.
	•	base64 <length> (or b64 <length>): Generate a Base64 string.
	•	alphanum <length> (or alphanumeric <length>): Generate an alphanumeric string.
	•	bin <length>: Generate a binary string.
	•	custom <set> <length>: Generate a string using a custom character set.

Data Management:

	•	save <filename>: Save generated output to a file on the SD card.
	•	read: Read and display the contents of the default output.txt file from the SD card.

> Kindly note that reading for now is only supported from the output.txt default file only. if you don't like this behaviour change it and send a pull request and I shall accept it.

Streaming:

	•	stream <command> <interval>: Continuously execute a command at a specified interval (in milliseconds).
	•	stop stream: Stop the currently active streaming mode.

Setup:

	•	setup: Configure the output destination (Serial, File, or API).

🔧 Usage

First Run:

	1.	Setup Configuration:
	•	Specify the output destination:
	•	Serial: Prints to Serial Monitor.
	•	File: Saves to a specified file on an SD card.
	•	API: Sends data to a configured API endpoint.
	•	For API output, configure:
	•	URL
	•	Payload format
	•	Authentication (Bearer or Basic)
	2.	Command Execution:
	•	Input commands via the Serial Monitor to generate IDs or manage data.
	3.	Streaming Mode:
	•	Use the stream command to automate ID generation.

🛠️ Error Handling

	•	If a required peripheral (e.g., SD card) is missing, a clear warning is displayed.
	•	Invalid commands or inputs are handled gracefully with detailed error messages.

🔄 Advanced Configuration

Modify the setup anytime using the setup command:
	•	Change the output destination.
	•	Reconfigure API settings.
	•	Update authentication details.

📌 Project Details

	•	Author: Ahmad Hamdi
	•	Platform: ESP32
	•	Libraries Used:
	•	Wire.h (I2C communication)
	•	SD.h and SPI.h (SD card operations)
	•	WiFi.h and HTTPClient.h (API integration)
	•	EEPROM.h (Persistent data storage)

🔍 Example Workflow:

	1.	Generate a UUID v4:

uuid v4


	2.	Stream random integers every 2 seconds:

stream int 16 2000


	3.	Save IDs to a file:

save output.txt


	4.	Read and print the file content:

read


	5.	Send data to an API:
Sending data to an API is an automated process that you should not use directly; instead, set the configuration to output to a specific API.

sendToAPI("https://example.com/api", "{\"id\":\"12345\"}", "bearer", "your-token")

📌 Notes:

	•	First Run Setup: Setup must be completed on the first run and can be modified later.
	•	Return to Normal Mode: Use stop stream to exit streaming mode.

Enjoy using Hamdiz Truly Random Multi-ID Generator (MIG)! 🎉
