## Mise à jour M015-M — seconde NUCLEO seulement

La chaîne seconde cible est qualifiée dans M015-L : SN `0032001F3235510C37333439`,
UID `001B00463842501820333157`. WHO7B et polling64 sont acquis ; ne pas les
rejouer sur code inchangé. Le contrat M015-M rend son firmware Flash jetable :
**aucune sauvegarde Flash2MiB ni restauration initiale répétée**. Les instructions
historiques K ci-dessous sont remplacées sur ce point, exclusivement pour ce banc.
Identité avant chaque flash, image/hash, programmation normale 08000000/verify,
readback et état final restent obligatoires. TR2 principal strictement exclu.

Le pilote et les exemples IIS3DWB ST ainsi que le SDK original restent inchangés.
`tools/instrument_hal.py` produit une copie HAL SPI dans le build, sur SHA original
pinné, avec un seul appel observateur à l'entrée CloseTransfer, hors boucle RX.
Les états SR/compteurs/offsets sont ainsi préservés avant nettoyage HAL.
Une lecture n'est acceptée que complète (taille, offset, restant, CS et erreurs),
en plus des contrôles HAL existants. 2048 snapshots maximum ; dépassement refusé.
`BENCH_SPI_OPT=0` reproduit Debug ; `=2` (défaut) optimise **seulement** ce module.
CPU16MHz, SPI mode0/2MHz, délais20ms et séquence capteur sont identiques.
Deux essais FIFO STATUS10 au maximum : diagnostic O0 puis candidat O2 seulement
si le premier refus est celui attendu. Arrêt sur anomalie nouvelle, aucune
qualification V1 ou perte globale déduite d'un succès borné.

# Protocole préparé — aucune exécution M015-K à ce stade

Statut **READY_FOR_PHYSICAL**, pas GO matériel. Le contrat diffère le matériel
jusqu'à confirmation du montage et identification de la seconde carte.

1. Après montage, relever seconde NUCLEO/révision, breakout/révision et orientation,
   câblage selon PINOUT, point GND identifié et continuités hors tension, puis
   VDD/VDDIO. Confirmer absence de FRAM/SD/RS485 et autres charges sur ce banc.
   L'utilisateur assure coupures et gestes physiques. Ne pas toucher le TR2.
2. Identifier indépendamment SN ST-LINK et UID96 de la **seconde** cible,
   tension et MCU. Refuser SN `003500463235510B37333439` et UID
   `004A003D3842501820333157`. Aucun choix «premier ST-LINK trouvé».
   Le planificateur n'effectue rien et ne prouve aucune identité.
3. Confirmer la chaîne qualifiée pour ce SN/cible/artefact. Créer des chemins
   de preuves nouveaux ; sauvegarder Flash2MiB (`08000000..08200000`) et RAM
   avant perturbation si utile, comparer deux lectures et conserver les SHA.
   Aucune modification boot/RDP/Option Bytes. Après backup, NORMALE SWD/SN
   explicite, write BIN `08000000`, verify, readback byte-identique.
   Une réussite CLI seule ne vaut pas validation capteur.
4. Finir tout CLI avant dernier reset GDB, puis GDB seul (procédure M013-D).
   Lire UID/Flash/image/`bench_core_hz`/faults, `bench_ready=1`, CS haut.
   Utiliser ELF exact. Aucun firmware principal M014-H ne doit être remplacé.
   Si la chaîne n'est pas qualifiée pour cette seconde cible : attendre
   l'autorisation indispensable d'AGENTS §11/12 avant tout flash/debug réel.
5. **WHO seul** : image `who/ST_STRICT`, un trigger autorisé4D31354B,
   détacher pendant exécution. Exiger résultat0/WHO7B/trace lectureunique,
   zéro écriture capteur et cleanup0. MauvaisWHO → arrêter, pas fallback/réessai.
6. **Polling ST** : seulement après WHO conforme, un run de `polling/ST_STRICT`.
   Exiger résultat0/cleanup0/64XYZ, readbacks conformes, SPI/HAL sans erreur,
   valeurs non toutes figées sous observation/sollicitation physique humaine
   connue, pas saturation massive. Conserver bruts, contexte réel et éventuelle
   stimulation ; ne pas supposer repos, gravité ou calibration.
7. **FIFO ST strict** : un run borné, constater le niveau tronqué prévu si
   watermark atteint ; garder le refus explicite ST_LEVEL. Ce refus du driver
   est attendu hors cible, mais aucune valeur matérielle ne doit être inventée.
   **FIFO STATUS10** est un run distinct seulement si autorisé au bilan de
   l'étape précédente ; conserver source/config identiques et déclarer l'écart
   de parsing. Pas relance opportuniste pour obtenir un succès.
   Exiger résultat0/cleanup0/512entrées réelles, XYZ2/timestamps4, niveaux/flags
   cohérents avant/après, pas FULL/overrun/latch, aucune perte démontrée par
   tags/parité/counters ; exporter timestamps bruts et vérifier leur évolution.
   512entrées mixtes ne valent pas512XYZ, encore moins fenêtre4096 V1.
8. Au premier refus matériel : arrêter la campagne, dump avant reset/reflash,
   conserver trace/erreur/payload rejeté, ne pas rectifier les octets ni réduire
   les contrôles. Stop500ms/100ms reset, erreurHAL20ms par appel ; constater
   CS haut après transaction. Cleanup peut échouer, jamais le masquer ; si
   nécessaire, intervention physique sûre ou restauration du backup sous
   procédure autorisée, pas écriture arbitraire.
9. Capturer `tools/capture.gdb` vers chemins exclusifs, seulement zones
   renseignées. Décoder FIFO avec `decode_capture.py`, comparer CSV↔raw et SHA.
   Lire le résultat avant de parler de validation. Templates GDB préparés
   hors cible, commandes déclenchantes commentées. Pas fichier fake mesuré.
10. Comparaison ultérieure isolé/busTR2 : fixer module, câblage, mode/fréquence,
    source et séquence, puis ne changer qu'un facteur identifié sous mission
    adaptée. Succès du banc isolé ne prouve pas panne du module initial ;
    horloge/CPU/mode/callbacks sont différents de J. Toute intervention sur le
    TR2 exige sauvegarde/restaurationM014-H et reprises autorisées distinctes.

Commandes CLI futures, **à compléter sans les exécuter aujourd'hui** :

```text
STM32_Programmer_CLI -c port=SWD sn=<SN_SECONDE_CONFIRMÉ> freq=8000 -u 0x08000000 2097152 <BACKUP_NOUVEAU>
STM32_Programmer_CLI -c port=SWD sn=<SN_SECONDE_CONFIRMÉ> freq=8000 -w <BIN_HASH_VÉRIFIÉ> 0x08000000 -v -rst
```

Ces formes sont celles de la chaîne projet déjà inspectée ; pas preuve de
qualification de la seconde cible. Relecture indépendante de l'identité,
backup et adressage avant exécution. Aucun mass erase ni HOTPLUG.
Rapport futur : source/artefact/SN/UID, observations séparées, limitations,
état final et éventuelle restauration du **backup de la seconde carte**.
