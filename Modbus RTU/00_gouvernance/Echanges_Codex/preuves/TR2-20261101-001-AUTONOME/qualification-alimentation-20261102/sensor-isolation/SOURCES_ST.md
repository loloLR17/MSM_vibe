# Références primaires vérifiées pendant l'analyse

- ST DS12569 Rev8, IIS3DWB : https://www.st.com/resource/en/datasheet/iis3dwb.pdf
  Tableau5 : SPI jusqu'à10MHz ; §9.28 : compteur32bits, résolution12,5µs.
  CTRL3_C décrit BDU et IF_INC. Une fréquence nominale sous10MHz ne qualifie
  ni le câblage ni l'échantillonnage électrique du banc.
- Driver ST : https://github.com/STMicroelectronics/iis3dwb-pid/blob/master/iis3dwb_reg.c
  iis3dwb_timestamp_raw_get lit4octets depuisTIMESTAMP0 et les assemble en little-endian.
  Le driver local suit cette forme ; aucune modification du vendorST ni hypothèse
  de correction de bits n'a été appliquée.

Ces documents ne démontrent pas la cause électrique des octets incohérents observés.
