<?
include("wsl_mail.php");

$g_sReportEmail = "sysmsg@spytech.cz";
mail($g_sReportEmail, "TEST: Testing mail().", "dummy");
	
WslSendCzechMail("tetikr@spytech.cz", "sysmsg@spytech.cz", "Èerveòouèký kùò", "Èerveòouèký kùò");
?>