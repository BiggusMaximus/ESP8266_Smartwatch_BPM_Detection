<!DOCTYPE html>
<html>

<body>
    <?php

    $servername = "localhost";
    $dbname = "heartbeat_db";
    $username = "root";
    $password = "";

    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    $sql = "SELECT id, bpm, reading_time FROM SensorData ORDER BY id DESC";

    echo '<table cellspacing="3" cellpadding="3">
            <tr> 
                <td>ID</td> 
                <td>BPM</td> 
                <td>Timestamp</td> 
            </tr>';

    if ($result = $conn->query($sql)) {
        while ($row = $result->fetch_assoc()) {
            $row_id = $row["id"];
            $bpm = $row["bpm"];
            $row_reading_time = $row["reading_time"];

            echo '<tr> 
                        <td>' . $row_id . '</td> 
                        <td>' . $bpm . '</td> 
                        <td>' . $row_reading_time . '</td> 
                    </tr>';
        }
        $result->free();
    }
    $conn->close();
    ?>
    </table>
</body>

</html>