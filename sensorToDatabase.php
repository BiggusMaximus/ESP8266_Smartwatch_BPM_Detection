<?php
$servername = "localhost";
$dbname = "heartbeat_db";
$username = "root";
$password = "";


$bpm = "";

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $bpm = test_input($_POST["bpm"]);
    $conn = new mysqli($servername, $username, $password, $dbname);
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }
    $sql = "INSERT INTO SensorData (bpm) VALUES ('" . $bpm . "')";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }

    $conn->close();
    
}

function test_input($data)
{
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
