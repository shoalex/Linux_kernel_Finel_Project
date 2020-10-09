<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of dbconnect
 *
 * @author shoal
 */
class dbconnect {
   private $connect;

    public function __construct() {
        $this->connect = new mysqli("localhost", "root", "", "kerneldb");
        $this->connect->set_charset("utf8");
        if ($this->connect->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }
    }
    
     public function insertrow($name,$id,$status) {
        if ($this->connect->query("INSERT INTO `attendance` (`date`, `time`, `name`, `id`, `status`) VALUES (date(now()), time(now()), '$name', '$id','$status');") === TRUE) {
            return TRUE;
        }
        return FALSE;
    }
    
    public function showallattendance() {
        $result = $this->connect->query("SELECT * FROM attendance order by idAttendance desc");
        return $result;
   
    }
}
