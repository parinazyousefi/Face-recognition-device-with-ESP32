# Face-recognition-device-with-ESP32


![facial recognition](https://github.com/user-attachments/assets/9ec234d7-5f37-4314-b342-5f119c08cd11)


<h2>Overview</h2>
<p>This project presents a sophisticated security system for server room access control, leveraging facial recognition technology. The system ensures that only authorized individuals gain entry to the server room, maintaining a comprehensive log of access attempts. The project utilizes two main microcontrollers, ESP32-CAM and Arduino UNO, along with various modules to achieve its functionality.</p>

<h2>Introduction</h2>
<p>Facial recognition technology has advanced significantly, allowing for precise identification based on facial features. In this project, we apply this technology to enhance the security of server rooms, crucial components of any organization's IT infrastructure. Each employee is granted four recognition attempts, with unauthorized entry attempts triggering email alerts to the administrator. Access records, including names and timestamps, are logged for monitoring and tracking purposes.</p>

<p>The developed system is accurate, reliable, and efficient, offering organizations control and oversight over their vital IT infrastructure. By employing cutting-edge hardware and algorithms, we've created a user-friendly solution that is easy to acquire and maintain.</p>

<h2>Functional Features</h2>
<h3>Turning on the System</h3>
<li>The user initiates the system by touching the LCD screen.</li>

![photo_2023-04-07_14-28-18](https://github.com/user-attachments/assets/3a441d13-1349-4bd5-b88d-c5a0f44dbb82)

<li>Instructions are displayed on the screen to guide the user through the process.</li>

![photo_2024-08-09_14-55-54](https://github.com/user-attachments/assets/1660022c-3251-4953-b95e-09d3a88dc752)

<h3>Facial Recognition</h3>
<li>The user faces the camera after reading the instructions.</li>
<li>The camera detects and authorizes the person based on the registered personnel list.</li>

![photo_2024-08-09_14-55-56](https://github.com/user-attachments/assets/a68dfbb7-efb3-47eb-8c55-71954e7a77f5)
![photo_2024-08-09_14-56-01](https://github.com/user-attachments/assets/0a17cb66-9d14-4675-8977-6d180c48476d)
<li>A status message is displayed, and the authorized user is logged into the system.</li>
<h3>Notification</h3>
<li>In case of denied access, an email is sent to the administrator, notifying them of the failed attempt.</li>

![Untitled video - Made with Clipchamp (1)](https://github.com/user-attachments/assets/0e1a576f-793a-4bf9-95d9-34bdb6489fc7)


<li>Daily email notifications containing access logs, specifying entry times, are sent to the administrator.</li>
