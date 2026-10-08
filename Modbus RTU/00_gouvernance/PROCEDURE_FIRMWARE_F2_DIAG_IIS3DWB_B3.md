# F2-DIAG — Indicateurs IIS3DWB réels, hors campagne

## Portée

Harness temporaire autorisé par le pilote, indépendant du runtime P8.
Il réutilise le pilote STM32 IIS3DWB, les types AcquisitionWindow,
SupervisionService (et son calculateur existant), puis modbus_project_b3.
Il ne compose ni CampaignService, ni SystemRuntime, ni serveur RTU.
Aucun START, append, checkpoint, finish ou succès persistant n'est simulé.
L'image B3 diagnostique n'est pas une publication de campagne de production.
Les contrats P8, mapping V1, E4 et formats persistants restent inchangés.

Le HEAD de départ est 18c9871481e714e05c93ee60d982c7293f488ba3 ;
les modifications locales F1/F2-DIAG doivent accompagner ce HEAD.
Le HEAD seul ne décrit donc pas le BIN diagnostique. Relever son SHA256,
celui de l'ELF correspondant et le diff utilisé avant toute mesure physique.

## Composition

- WHO_AM_I attendu : 0x7B ; configuration XYZ, 26667 Hz nominal, ±2 g.
- Taille : 4096 échantillons valides, taille minimale admise par V1.
- Accumulation constante en mémoire : sommes carrées et maxima, aucun buffer XYZ complet.
- Polling STATUS/XYZ via le pilote validé F1 ; absence de nouvelle donnée : attente.
- Une attente sans échantillon pendant 1000 ms provoque un arrêt diagnostique,
  pas une politique de retry du runtime P8.
- Erreur source, échantillon invalide ou overflow : aucune nouvelle image valide.
- Une seule fenêtre par boot contrôlé, puis conservation des résultats.
- Les mesures sont en mg et incluent la gravité : aucun filtrage DC ou FFT ajouté.
- La durée B3 est la durée réellement mesurée, pas 4096/26667 supposé.
- Le polling ne démontre ni absence de pertes ni cadence soutenue à 26667 Hz.

Aucune configuration active persistante, seuil d'alarme ou horodatage civil
n'est créé. Les faits de seuils ne sont pas exploités ; seules les projections
B3 existantes sont utilisées. La disponibilité de cette image ne prouve aucune
readiness de stockage ou campagne. Le harness reste avant les essais FRAM/SD
et n'effectue aucun accès persistant.

## Build

Depuis la racine du repository main :

```bash
STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 ./tr2_validate.sh
sha256sum 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin' \
          'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.elf'
```

Utiliser exclusivement ce BIN et cet ELF, pas ceux du worktree E4.
Tout flash nécessite l'autorisation explicite de la mission matérielle.
La microSD doit rester retirée pour cette qualification diagnostique.
Ne pas maintenir manuellement RESET pendant la connexion Under Reset :
cette situation a provoqué DEV_TARGET_HELD_UNDER_RESET sur ce poste.

## Câblage

Référence : CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md.
VDD et VDDIO sur 3V3, masse commune ; CS PC9/CN8-4 ; SCK PC10/CN8-6 ;
MISO PC11/CN8-8 ; MOSI PC12/CN8-10. INT1/INT2 non utilisés.

## Observation GDB après flash autorisé

Charger l'ELF exact correspondant au BIN vérifié. Utiliser la chaîne ST-LINK
qualifiée du projet. Dans une session GDB déjà connectée et CPU arrêté :

```gdb
set pagination off
hbreak *Reset_Handler
hbreak Iis3dwbF2DiagPublished
monitor reset hardware
monitor halt
maintenance flush register-cache
info registers pc sp
x/2wx 0x08000000
info breakpoints
```

Un seul reset contrôlé : PC doit correspondre à Reset_Handler et SP au
premier mot des vecteurs. Ne pas continuer si ce contrôle échoue.
Supprimer uniquement le breakpoint Reset_Handler (son numéro observé avec
info breakpoints), puis :

```gdb
continue
p/x *(unsigned char *)&tr2_iis3dwb_whoami
p/u *(unsigned int *)&tr2_iis3dwb_whoami_status
p/u *(unsigned int *)&tr2_iis3dwb_f1_init_result
p/u *(unsigned int *)&tr2_iis3dwb_config_status
p/u *(unsigned int *)&tr2_iis3dwb_f1_start_result
p/u *(unsigned int *)&tr2_iis3dwb_sample_status
p/u *(unsigned int *)&tr2_iis3dwb_f1_sample_count
p/u *(unsigned int *)&tr2_iis3dwb_f2_result
p/u *(unsigned int *)&tr2_iis3dwb_f2_ready
x/48uh &tr2_iis3dwb_f2_b3_registers
```

En cas d'absence d'arrêt au point de publication, interrompre avec Ctrl-C
pour lire les mêmes diagnostics ; aucun reset répété ni résultat fictif.
Succès logiciel sur cible : WHO_AM_I=0x7B, statuts=0, compteur=4096,
f2_result=0 et f2_ready=1. Ces valeurs ne sont pas des résultats déjà obtenus.
Le CPU reste arrêté pendant la lecture du tableau cohérent.

Le tableau commence au registre conceptuel 3000. Chaque uint32 est MSW/LSW :

| Adresse V1 | Indicateur | Unité |
|---|---|---|
| 3008–3009 | séquence diagnostique (1 pour ce boot) | compteur |
| 3010–3011 | durée mesurée | ms |
| 3012–3013 | échantillons valides | compteur |
| 3014–3015 | RMS global | mg |
| 3016–3017 | Peak global | mg |
| 3018–3019 | RMS X | mg |
| 3020–3021 | RMS Y | mg |
| 3022–3023 | RMS Z | mg |
| 3024–3025 | Peak X | mg |
| 3026–3027 | Peak Y | mg |
| 3028–3029 | Peak Z | mg |

Pour lire directement RMS global sans types de debug :

```gdb
p/u ((unsigned int)*((unsigned short *)&tr2_iis3dwb_f2_b3_registers+14)<<16) | *((unsigned short *)&tr2_iis3dwb_f2_b3_registers+15)
```

Appliquer la même formule aux indices 16,18,20,22,24,26,28.
B3 contient des RMS/Peak non signés, pas les XYZ instantanés signés.

## Qualification physique ultérieure

Consigner BIN/ELF SHA256, WHO_AM_I, statuts, durée, compteur, RMS/Peak et flags.
Une première fenêtre immobile doit produire des valeurs réelles plausibles.
Une seconde fenêtre nécessite un nouveau démarrage contrôlé explicitement
autorisé. Pendant cette fenêtre, l'opérateur peut solliciter doucement le
capteur pour observer les variations RMS/Peak ; le harness ne déclenche
aucune manipulation physique. Une rotation statique modifie la répartition
par axes, pas nécessairement la norme globale.

Aucune preuve physique F2-DIAG n'est consignée comme déjà acquise ici.
Les tests hôte utilisent des données déterministes comme preuves logicielles
uniquement. Cette qualification ne démontre ni RTU/RS-485 physique,
ni stockage, ni cadence garantie, ni calibration métrologique.
