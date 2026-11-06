"""Explicit frozen V27 whitelist. Counts are for BOTH prototypes together."""
from pathlib import Path
P=Path(__file__).resolve().parents[1];V27=P.parent;SHIELD=V27.parent
MID='TR2-20261106-SHIELD-V27-PACK-MORVAN-PDF'
FROZEN='50af554db63ab76664df63565f9203e04f2d963e'
ZIP_NAME='TR2_V27_ABS_MORVAN_IMPRESSION_SIMPLE_2_PROTOTYPES.zip'
OLD_ZIP=V27/'pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip'
OLD_SHA='7f21033ffeb50732b3223037159ed1aecf0052235d98e4e7c62ca02c599bbb8d'
# name, quantity, plain-language role; no glob and no historical directory sweep.
WHITELIST=[
 ('01_base_ABS.stl',2,'Caisse'),
 ('02_couvercle_ARNACO_USER.stl',2,'Couvercle'),
 ('03_facade_STLINK_LED_nappe.stl',2,'Façade avec fenêtre'),
 ('06_passe_cable_RS485.stl',2,'Passage de câble'),
 ('07_cadre_vitre.stl',2,'Cadre de fenêtre'),
 ('10_serre_nappe.stl',2,'Bride de nappe plate'),
 ('11_serre_cable_RS485.stl',2,'Bride de câble rond'),
 ('13_D126.stl',8,'6 utiles + 2 en réserve'),
 ('15_retenue_Nucleo.stl',6,'Retenue de carte'),
 ('20_selle_FRAM_H10p6.stl',2,'Support mémoire 10,6'),
 ('20_selle_FRAM_H11p2.stl',2,'Support mémoire 11,2'),
 ('20_selle_FRAM_H11p8.stl',2,'Support mémoire 11,8'),
 ('20_selle_SD_H10p6.stl',2,'Support lecteur 10,6'),
 ('20_selle_SD_H11p2.stl',2,'Support lecteur 11,2'),
 ('20_selle_SD_H11p8.stl',2,'Support lecteur 11,8'),
 ('30_etrier_MKI_ABS.stl',2,'Retenue du capteur'),
 ('31_etrier_Click_ABS.stl',2,'Retenue du module'),
 ('32_patin_MKI_avant_ABS.stl',2,'Petit patin avant'),
 ('33_patin_MKI_arriere_ABS.stl',2,'Petit patin arrière'),
 ('34_patin_Click_ABS.stl',4,'Petit patin de retenue'),
 ('40_cage_USER_captive.stl',2,'Guide du poussoir'),
 ('41_USER_pointe_Z8p8.stl',2,'Poussoir, version 1'),
 ('41_USER_pointe_Z9p6.stl',2,'Poussoir, version 2'),
 ('41_USER_pointe_Z10p4.stl',2,'Poussoir, version 3'),
 ('50_coupon_ABS.stl',1,'Petit échantillon ABS')]
PDFS=['01_LIRE_EN_PREMIER_MORVAN.pdf','02_VUES_DES_PIECES.pdf']
assert len(WHITELIST)==25 and len({n for n,_,_ in WHITELIST})==25
assert sum(q for _,q,_ in WHITELIST)==61
