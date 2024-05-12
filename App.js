import { StatusBar } from "expo-status-bar";
import { useState, useRef, useEffect } from "react";
import {
  Button,
  StyleSheet,
  Text,
  View,
  Alert,
  ScrollView,
} from "react-native";
import { BleManager } from "react-native-ble-plx";
import { btoa, atob } from "react-native-quick-base64";
import { colors } from "./const.js";
import { TouchableOpacity, Image } from "react-native";

import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";

const bleManager = new BleManager();

const SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";

const CustomButton = ({ onPress, color, text, style }) => (
  <TouchableOpacity
    onPress={onPress}
    style={[styles.button, { backgroundColor: color }, style]}
  >
    <Text style={styles.buttonTextBlack}>{text}</Text>
  </TouchableOpacity>
);

export default function App() {
  const [deviceId, setDeviceId] = useState(null);

  const [connectionStatus, setConnectionStatus] = useState("Searching...");

  const deviceRef = useRef(null);

  const [isOn, setIsOn] = useState(false);

  const [score, setScore] = useState(0);

  const toggleSwitch = () => {
    // Invert the current state
    const newState = !isOn;
    setIsOn(newState);

    // Call writeToCharacteristic with "A" if the newState is true (ON), else with "B"
    writeToCharacteristic(newState ? "A" : "B");
  };

  const writeToCharacteristic = async (character) => {
    if (!deviceRef.current) {
      console.error("Device is not connected");
      return;
    }

    try {
      const serviceUUID = "0000ffe0-0000-1000-8000-00805f9b34fb"; // Change this if needed
      const characteristicUUID = "0000ffe1-0000-1000-8000-00805f9b34fb";

      // Convert the character to a byte array
      const data = new Uint8Array([character.charCodeAt(0)]).buffer;

      // Encode the data in base64
      const base64Data = btoa(String.fromCharCode(...new Uint8Array(data)));
      // Write to the characteristic
      for (let i = 0; i < 1; i++) {
        await deviceRef.current.writeCharacteristicWithoutResponseForService(
          serviceUUID,
          characteristicUUID,
          base64Data
        );
        console.log(`Sent '${character}' to characteristic ${i + 1} times`);
      }
    } catch (error) {
      console.error("Failed to write to characteristic:", error);
    }
  };

  const searchAndConnectToDevice = () => {
    bleManager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.error(error);
        setConnectionStatus("Error searching for devices");
        return;
      }
      if (device.name === "GLIDEGLOW") {
        bleManager.stopDeviceScan();
        setConnectionStatus("Connecting...");
        connectToDevice(device);
      }
    });
  };

  useEffect(() => {
    searchAndConnectToDevice();
  }, []);

  const connectToDevice = (device) => {
    return device
      .connect()
      .then((device) => {
        // console.log(device);
        setDeviceId(device.id);
        setConnectionStatus("Connected");
        deviceRef.current = device;
        return device.discoverAllServicesAndCharacteristics();
      })
      .then((device) => {
        return device.services();
      })
      .then((services) => {
        let service = services.find((service) => service.uuid === SERVICE_UUID);
        console.log(service);
        return service.characteristics();
      })
      .then((characteristics) => {
        console.log(characteristics);
        // Subscribe to the characteristic that sends the score
        const scoreCharacteristicUUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
        deviceRef.current.monitorCharacteristicForService(
          SERVICE_UUID,
          scoreCharacteristicUUID,
          (error, characteristic) => {
            if (error) {
              console.error("Error subscribing to score updates:", error);
              return;
            }
            // Assuming the score is sent as a simple string or number
            // const receivedScore = parseInt(atob(characteristic.value), 10);
            const receivedScore = parseInt(atob(characteristic.value), 10);
            setScore(receivedScore);
            console.log(receivedScore);
            console.log(characteristic.value);
          }
        );
      })
      .catch((error) => {
        console.log(error);
        setConnectionStatus("Error in Connection");
      });
  };

  useEffect(() => {
    const subscription = bleManager.onDeviceDisconnected(
      deviceId,
      (error, device) => {
        if (error) {
          console.log("Disconnected with error:", error);
        }
        setConnectionStatus("Disconnected");
        console.log("Disconnected device");
        if (deviceRef.current) {
          setConnectionStatus("Reconnecting...");
          connectToDevice(deviceRef.current)
            .then(() => setConnectionStatus("Connected"))
            .catch((error) => {
              console.log("Reconnection failed: ", error);
              setConnectionStatus("Reconnection failed");
            });
        }
      }
    );
    return () => subscription.remove();
  }, [deviceId]);

  return (
    <ScrollView style={styles.container}>
      <Text style={styles.buttonTextBlack}>Filler</Text>
      <Image
        source={require("./ButtonPictures/Title1.png")}
        style={styles.image}
      />

      <View style={styles.row}>
        <TouchableOpacity
          style={isOn ? styles.buttonOn : styles.buttonOff}
          onPress={toggleSwitch}
        >
          <Text style={styles.buttonText}>{isOn ? "ON" : "OFF"}</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.info}>
        <Text style={styles.status}>Status: {connectionStatus}</Text>
        {/* Display the score here */}
      </View>

      <View style={styles.horizontalLine} />

      <View style={styles.header}>
        <Text style={styles.subheader}>Standard Mode</Text>
      </View>
      <View style={styles.row}>
        {/* <TouchableOpacity
          onPress={() => {
            writeToCharacteristic("C");
            Alert.alert("Standard Mode Actived");
          }}
        >
          <Text style={styles.subheader}>↓↓↓</Text>
          <Image
            source={require("./ButtonPictures/Button1.png")}
            style={{ width: 50, height: 50 }}
          />
        </TouchableOpacity> */}
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("C");
            Alert.alert("Standard Mode Activated");
          }}
        >
          <Text style={styles.buttonTextBlack}>SM</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.header}>
        <Text style={styles.subheader}>Animations</Text>
      </View>
      <View style={styles.row}>
        {/* <Button
          title="A1"
          onPress={() => {
            writeToCharacteristic("D");
            Alert.alert("Animation 1 Actived");
          }}
        ></Button> */}
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("D");
            Alert.alert("Animation 1 Actived");
          }}
        >
          <Text style={styles.buttonTextBlack}>A1</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("E");
            Alert.alert("Animation 2 Actived");
          }}
        >
          <Text style={styles.buttonTextBlack}>A2</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("F");
            Alert.alert("Animation 3 Actived");
          }}
        >
          <Text style={styles.buttonTextBlack}>A3</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.header}>
        <Text style={styles.subheader}>Games</Text>
      </View>
      <View style={styles.row}>
        {/* <Text style={styles.subheader}>Games:</Text> */}
        <TouchableOpacity
          style={styles.button2}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("G");
            Alert.alert("Reaction Game Activated");
          }}
        >
          <Text style={styles.buttonTextBlack}>Reaction Game</Text>
        </TouchableOpacity>
      </View>
      <View style={styles.header}>
        <Text style={styles.subheader}>Game Score: {score}</Text>
      </View>

      <Text style={styles.buttonTextBlack}>Filler</Text>

      <View style={styles.horizontalLine} />

      <View style={styles.header}>
        <Text style={styles.subheader}>Brightness</Text>
      </View>
      <View style={styles.row}>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("H");
          }}
        >
          <Text style={styles.buttonTextSmall}>25%</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("I");
          }}
        >
          <Text style={styles.buttonTextSmall}>50%</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("J");
          }}
        >
          <Text style={styles.buttonTextSmall}>75%</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("K");
          }}
        >
          <Text style={styles.buttonTextSmall}>100%</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.horizontalLine} />

      <View style={styles.header}>
        <Text style={styles.subheader}>LED Color</Text>
      </View>
      <View style={styles.row}>
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("L");
          }}
          color="red" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("M");
          }}
          color="green" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("N");
          }}
          color="blue" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("O");
          }}
          color="pink" // Specify the unique color here
        />
      </View>

      <View style={styles.row}>
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("P");
          }}
          color="orange" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("Q");
          }}
          color="#90EE90" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("R");
          }}
          color="#ADD8E6" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("S");
          }}
          color="purple" // Specify the unique color here
        />
      </View>

      <View style={styles.row}>
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("T");
          }}
          color="#FFD580" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("U");
          }}
          color="yellow" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("V");
          }}
          color="teal" // Specify the unique color here
        />
        <CustomButton
          onPress={() => {
            if (!isOn) {
              setIsOn(true); // Turn on the "ON/OFF" button if it's off
            }
            writeToCharacteristic("W");
          }}
          color="violet" // Specify the unique color here
        />
      </View>

      <View style={styles.header}>
        <Text style={styles.footer}>© GlideGlow Group 2024</Text>
      </View>

      <Text style={styles.buttonTextBlack}>Filler</Text>
      <Text style={styles.buttonTextBlack}>Filler</Text>
      <Text style={styles.buttonTextBlack}>Filler</Text>
      <Text style={styles.buttonTextBlack}>Filler</Text>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    Flex: 1, // Add this line to fill the whole screen
    padding: 50,
    backgroundColor: "black", // Set the background color to black
  },
  header: {
    justifyContent: "center",
    alignItems: "center",
  },
  subheader: {
    fontSize: 25,
    fontWeight: "bold",
    color: "white", // Set the text color to white
  },

  row: {
    margin: 20,
    flexDirection: "row",
    justifyContent: "space-around",
  },

  info: {
    margin: 20,
    justifyContent: "center",
    alignItems: "center",
  },
  status: {
    fontSize: 17,
    fontWeight: "400",
    color: "white", // Set the text color to white
  },

  image: {
    width: 250, // Set the width as needed
    height: 120, // Set the height as needed
    alignSelf: "center",
    // Add other styling properties if needed
  },

  button: {
    width: 50, // Set the width as needed
    height: 50, // Set the height as needed
    backgroundColor: "#fec7dc", // Color for the ON state
    // add other styling for the button
    borderRadius: 20, // Half of the width or height to create a circle
    justifyContent: "center", // Center the content horizontally
    alignItems: "center", // Center the content vertically
  },
  button2: {
    width: 200, // Set the width as needed
    height: 50, // Set the height as needed
    backgroundColor: "#fec7dc", // Color for the ON state
    // add other styling for the button
    borderRadius: 20, // Half of the width or height to create a circle
    justifyContent: "center", // Center the content horizontally
    alignItems: "center", // Center the content vertically
  },
  buttonTextBlack: {
    fontSize: 30,
    fontWeight: "bold",
    color: "black", // Text color
    // add other styling for the text
  },
  buttonTextSmall: {
    fontSize: 15,
    fontWeight: "bold",
    color: "black", // Text color
    // add other styling for the text
  },

  buttonOn: {
    width: 50, // Set the width as needed
    height: 50, // Set the height as needed
    backgroundColor: "green", // Color for the ON state
    // add other styling for the button
    borderRadius: 20, // Half of the width or height to create a circle
    justifyContent: "center", // Center the content horizontally
    alignItems: "center", // Center the content vertically
  },
  buttonOff: {
    width: 60, // Set the width as needed
    height: 50, // Set the height as needed
    backgroundColor: "red", // Color for the OFF state
    // add other styling for the button
    borderRadius: 20, // Half of the width or height to create a circle
    justifyContent: "center", // Center the content horizontally
    alignItems: "center", // Center the content vertically
  },
  buttonText: {
    fontSize: 30,
    fontWeight: "bold",
    color: "white", // Text color
    // add other styling for the text
  },

  horizontalLine: {
    height: 1, // Thin line
    backgroundColor: "white", // Line color
    marginVertical: 10, // Space above and below the line
  },

  footer: {
    fontSize: 15,
    fontWeight: "200",
    color: "white", // Text color
    // add other styling for the text
  },
});
