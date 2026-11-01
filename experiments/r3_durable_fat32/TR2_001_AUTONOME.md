# TR2-001 autonome — profil expérimental réducteur

**Préparé, non qualifié : ne pas lancer la campagne longue.** Mission
`TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR`, exclusivement SN
`003500463235510B37333439`, UID `004A003D/38425018/20333157`.
TR2-002 et sa carte historique sont exclus de cette intervention.

## Code et validation courte

Profil explicite `R3_TR2_001_AUTONOMOUS=ON`, `R3_ENDURANCE_8H=ON`,
`R3_ENDURANCE_1H=ON`, `R3_SPI_DUPLEX=ON`, `R3_QPC_WITNESS=OFF`.
SPI3 8 MHz, pipeline R3 FIFO/FIR107Q15 /10/XYZ int16/v3 conservé.
FS logique3 = physique ±16 g ; contrôle CTRL1_XL bits0x0C =0x04,
488 µg/count dans l'en-tête et le lecteur PC. Les anciennes archives
±8 g restent à244 µg/count. Aucune suppression DC implicite ici.

Pour les seuls nouveaux tests hôte :

```sh
cmake -S experiments/r3_durable_fat32 -B /tmp/tr2-001-host -DR3_TR2_001_HOST_TESTS=ON
cmake --build /tmp/tr2-001-host
ctest --test-dir /tmp/tr2-001-host -V
```

Build ARM avec le toolchain Linux du dépôt et CubeU5v1.9.0 ; ajouter
les options ci-dessus, un préfixe neuf trois caractères, et
`R3_CAPTURE_DURATION_US=3000000` pour le gate court. Le gate doit être
qualifié avant toute image longue. Durée finale prévue30600000000µs
MCU nominales, sans promesse d'heure SI. Aucun long START Codex.
La préallocation est bornée131072B pour≤5s,2Mio pour≤60s,640Mio au-delà.

`R3_TR2_001_DISCOVERY=ON` : démarrage automatique d'un contrôle **lecture
seule**, sans capture, puis arrêt volontaire avant le gate d'écriture.
Le binding zéro de `tr2_001_card.h` est volontaire : toutes les écritures
restent interdites tant que CID/capacité réels512Go ne sont pas relevés.
Pas de fallback vers l'ancienne32Go. MBR simple FAT32/type0B ou0C,
secteur512, bornes et BPB vérifiés avant autorisation d'écriture ;
montage FatFs, espace libre et absence du nom contrôlés avant WAIT.
Le formatter est indisponible dans ce profil.

## Utilisation prévue — uniquement après qualification publiée

Au boot : initialisation/carte/capteur/calibration puis WAIT. B1/PC13
est une entrée avec pulldown. Relâcher le bouton au démarrage, attendre
WAIT puis maintenir USER environ1,6s. Un seul départ par boot ; les
rebonds et les appuis suivants ne relancent rien. Aucun déclencheur RAM
n'est disponible dans le profil autonome. Un nom existant provoque un
refus avant WAIT, jamais un écrasement ou une reprise silencieuse.

LED prévues : LD1 vertePC7 clignotante = WAIT ; LD2 bleuePB7 clignotante
= initialisation ou capture (cadences distinctes) ; verte fixe = fin
avec footer réussi ; LD3 rougePG2 clignotante = erreur. Code fondé sur
[le BSP officiel ST pour NUCLEO144](https://github.com/STMicroelectronics/stm32u5xx-nucleo-bsp/blob/main/stm32u5xx_nucleo.h).
**Ces observations visuelles ne sont pas encore réalisées sur le banc.**

STOP temporel, drain et footer sont autonomes. Erreur/rails/clips :
pas de footer COMPLETE. En erreur, conserver la carte et les diagnostics,
ne pas supposer l'ODR arrêtée si STOP a échoué. Ne jamais retirer la SD
pendant l'activité. Après fin confirmée et débogueur détaché : couper
l'alimentation puis transférer la carte au PC ; refuser réparation et
formatage. Le lecteur strict doit vérifier le fichier complet utile et
le SHA producteur avant d'annoncer une acquisition complète.

## Alimentation — préparation, manipulation utilisateur requise

Utilisateur : powerbank sans marque22000mAh, port prévuCN15. La capacité
nominale seule ne démontre pas huit heures ni le maintien en faible charge.
Selon [ST UM2861](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf),
CN15/5V_USB_C correspond à JP6[7–8] sur MB1549 documentée ; la position
CN1/ST-LINK[1–2] dépend d'un hôte USB et ne démontre pas l'autonomie.
**Vérifier révision/sérigraphie/position réelles avant toute instruction de
changement ; aucun jumper n'a été déplacé ou confirmé ici.**
Ne pas utiliser le mode CN1 chargeur[9–10] avec un PC connecté.
La qualification proposée est un gate court puis, si nécessaire,60s
sur CN15/powerbank sans ST-LINK et avec START physique USER, suivi
lecteur natifPC. Elle ne certifiera pas la capacité huit heures.

Installer batterie/boîtier loin des pièces mobiles, chaleur, huile et
parties électriques nues ; fixation rigide du capteur et retenue des
câbles à confirmer par l'utilisateur avant sa campagne physique.
L'anomalie N8G historique reste non résolue ; aucune immunité ni
endurance huit heures de ce code partagé n'est revendiquée.
