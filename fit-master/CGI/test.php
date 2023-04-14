<?php
// Check if form is submitted using POST method
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  // Handle form data submitted using POST method
  $name = $_POST['name'];
  $email = $_POST['email'];
  echo "Hello, $name! Your email address is $email.";
} else if ($_SERVER['REQUEST_METHOD'] == 'GET') {
  // Handle form data submitted using GET method
  $name = $_GET['name'];
  $email = $_GET['email'];
  echo "Hello, $name! Your email address is $email.";
}
?>