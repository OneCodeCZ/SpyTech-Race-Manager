<?
///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - Mail Utilities
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 08 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
function __EncodeEmailSubject($subject,$encoding="windows-1250")
{
  // BUG: According to the RFCxxxx the line should be no longer the 75 chars
  // See PHP docs on details.

  $len = strlen($subject);
  $enc = "=?".$encoding."?Q?";
  for ( $i=0 ; $i<$len ; $i++ )
  {
    $c = ord(substr($subject,$i,1));
    if ($c==32) $c = ord('_');
    // NOTE: '=' must be escaped
    $enc .= $c<128 && $c!=0x3d ? chr($c) : sprintf("=%2X",$c);
  }
  return $enc . "?=";
}

/*****************************************************************************/
function __EncodeEmailBody($s)
{
  $len = strlen($s);
  $enc = "";
  for ( $i=0 ; $i<$len ; $i++ )
  {
    $c = ord(substr($s,$i,1));
    // NOTE: '=' must be escaped
    $enc .= $c<128 && $c!=0x3d ? chr($c) : sprintf("=%2X",$c);
  }
  return $enc;
}

/*****************************************************************************/
function WslSendCzechMail($sFrom, $sTo, $sSubject, $sContent)
{
  return mail($sTo, 
    __EncodeEmailSubject($sSubject), 
    __EncodeEmailBody($sContent),
    "Mime-Version: 1.0".
	    "\r\nFrom: ".$sFrom.
      "\r\nContent-type: text/plain; charset=\"windows-1250\"".
      "\r\nContent-Transfer-Encoding: quoted-printable"
    );
}

?>