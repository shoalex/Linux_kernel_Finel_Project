<?php
$time=date("H:i");
$time=explode(":",$time);

if($time>=12)
{
    echo "out";
}
/*
require_once './db/dbconnect.php';
$db=new dbconnect();
if(!empty($_GET))
{
    if(!isset($_GET['name']))
    {
        echo 'missing name';
        return;
    }
    if(!isset($_GET['id']))
    {
        echo 'missing id';
        return;
    }
    $db->insertrow($_GET['name'], $_GET['id']);
    echo 'row added';
}
 else {
    
?>
<!DOCTYPE html>
<!DOCTYPE html>
<html lang="en">
<head>
  <title>Bootstrap Example</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js"></script>
</head>
<body>

<div class="container">
  <h2>Kernel Table</h2>           
  <table class="table table-striped">
    <thead>
      <tr>
        <th>Date</th>
        <th>Time</th>
        <th>name</th>
        <th>id</th>
      </tr>
    </thead>
    <tbody>
        <?php 
        $res=$db->showallattendance();
        while ($row = mysqli_fetch_array($res)) { ?>
        <tr>
        <td><?php echo $row['date'] ?></td>
        <td><?php echo $row['time'] ?></td>
        <td><?php echo $row['name'] ?></td>
        <td><?php echo $row['id'] ?></td>
      </tr>
       <?php }
        
        ?>
      
    </tbody>
  </table>
</div>

</body>
</html>

 <?php }*/ ?>