---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: 6c5ecb47e0c7bdbf83f9541d585a79d3dc9aaf24
created_at_utc: 2026-11-01T16:57:14.405923+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# TR2-001 autonome ±16 g — checkpoint logiciel ; carte SD non initialisée

**Pas prêt pour la campagne.** Logiciel autonome préparé et builds ARM courts/long
réussis ; première découverte physique : WHO_AM_I **0x7B**, mais **CMD0 sans réponse
(R1=0xFF, SD_TIMEOUT2)**. Aucun CID/CSD/capacité/MBR512Go obtenu, zéro écritureSD,
aucun enregistrement. Ne pas confondre le build logiciel avec une qualification.
La remise sous tension à froid de **001 seulement** a été demandée à l'utilisateur ;
retour encore attendu au moment de ce checkpoint. Poursuite sous le même contrat.

## Préservation et identité

- Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-autonome`, branche `codex/tr2-001-autonome`,
  démarrage propre depuis origin/main79b952d. Dépôt historique/main82169af et worktree
  R3a8817bf préservés, aucune résolution de leur divergence. Deux stashes historiques
  6251b988 et f3b829e6 inchangés. Aucun reset/clean/restore/rebase/merge/force push Git.
- Seule cible physique utilisée : SN003500463235510B37333439,
  UID004A003D/38425018/20333157 relu par GDB avant flash. TR2-002 hors opérations.
- Les deux sauvegardes utilisateur2Mio ont été relues et hachées effectivement avant
  flash : SHA256 `31926EB263DB85917E7F4C04B95109C2FB174102100349A7DA532CA3BD9807C5`.
  Emplacement Windows `C:\Users\lolo\Downloads\MSM_TR2_Backups\TR2-001_20261101` ;
  `backup-verification.json` reproduit les tailles/hashes observés, pas un hash supposé.
- L'inventaire Windows montre la carte historique32Go dans le lecteur natif ; aucun
  fichier de cette carte n'a été ouvert. Utilisateur confirme séparément **512Go dans
  TR2-001, câblage SPI2 et polarité/3,3V vérifiés**. Le FAT32/label512 GO reste une
  information utilisateur non vérifiée via le banc à ce stade.

## Réalisé en logiciel

Commits `64ee78c410659b6fdc44c78f96c2516bfd877888` et
`6c5ecb47e0c7bdbf83f9541d585a79d3dc9aaf24` : profil explicite001 dans `experiments/r3_durable_fat32`,
réemploi FIFO/FIR107Q15 /10/v3, SPI3duplex8MHz ; profil002 et V1 conservés.

- FS logique3/physique±16g, 488µg/count dans en-tête et lecteur ; les anciennes données
  ±8g restent244µg/count. FS16 n'est accepté que dans v3, sans nouvelle signification
  implicite des formats antérieurs. Rails/clips, CRC, SHA producteur, séquences,
  session, temps, footer et seuils1000/2500/5000µs conservés. DC conservée ici.
- Boot sûr→initialisation/capteur/SD/calibration→contrôle espace/nom→WAIT ; USER/PC13
  entrée pulldown, debounce50ms et appui1500ms après stabilisation, relâchement requis,
  verrou de départ unique, aucun START RAM ou à l'alimentation dans le profil autonome.
  LED verte WAIT, bleue ACTIVE, verte fixe COMPLETE et rouge ERROR préparées d'après
  BSP officiel ST. Arrêt FIFO tenté en cas d'échec calibration ; traces de défaut
  conservées. Ces transitions et LED restent à observer matériellement.
- STOP/flush/footer autonomes, durée cible longue30600000000µsMCU ; pas de garantieSI,
  pas de dépendance QPC/UART/Windows/GDB. Préallocation640Mio longue et tailles réduites
  pour gates3s/60s. Un nom existant est refusé, jamais réutilisé automatiquement.
- Gate carte512 exact prévu dans `src/tr2_001_card.h` : **binding zéro volontaire** tant
  que CID/capacité inconnus, donc aucune écriture autorisée. MBR FAT32 simple/type0B/0C,
  BPB512octets/cluster/plage contrôlés avant gate ; pas d'exFAT ou fallback32Go.
  Formatter désactivé dans001. Ni cette géométrie ni le libre n'ont été mesurés sur512Go.

## Validations effectivement exécutées

Validation proportionnée au harness isolé : builds CMake ARM dédiés ; pas de suite
historique générale ni de `tr2_validate.sh` canonique sans apport pour ce contrat court.
Aucun test déjà acquis M014/R3/N8G rejoué. Les branches par défaut du backend restent
inchangées ; le lecteur a ses tests ciblés et un oracle exhaustif de l'ancienne échelle.

1. Trois tests hôte dédiés PASS : bouton/rebond/maintien au boot/appuis répétés ;
   MBR/BPB/ranges/overflow/exFAT/mauvaise identité ; parsing/échelles FS8 etFS16/CRC.
   Fixtures synthétiques seulement : pas de qualification physique512Go.
2. Extension du test lecteur après restriction v3 : neuf cas + les65536int16 de l'échelle
   FS8 comparés à l'ancienne conversion, PASS (`host-reader-extended.log`). Les deux
   autres tests inchangés n'ont pas été répétés sans raison.
3. Build ARM final source `6c5ecb47e0c7bdbf83f9541d585a79d3dc9aaf24` PASS pour gate3s/A1A et
   variante longue/R1A **non flashées**. LongBIN51540B SHA256
   `5896c473e8d571c60bcddcdaec7779cfab17ab89e8a2d21773ed06f2e746ad40`.
   Tailles/ELF/BIN/CMakeCache et SHA dans `artifact-manifest.json` ; fichiers volumineux
   conservés localement, pas Git. Absence des symboles f_mkfs/witness_uart/r3_command
   contrôlée dans les deux images autonomes. Warnings nosys de newlib consignés,
   aucun warningC ignoré ; un premier échec misleading-indentation corrigé et conservé.

## Première observation physique, distincte du logiciel préparé

Un flash standard de découverte **lecture seule**, adresse0x08000000, CLI `-v -rst`,
SN001 explicite, téléchargement vérifié. Aucun mass erase/OptionBytes/déprotection.
Image51068B SHA256 `1283f70a60b68e7de03298014f3c23f79d987c72023dad1b26839a8b6eb224bc` ;
base79b952d avec adaptation de découverte alors non commitée, **pas** l'image autonome
finale6c5ecb47. Son BIN a été préservé par relectureGDB et hash intégral exact.
Trois sessionsGDB : identité préalable, observation, préservation readback ; détachées,
port qualifié23336. Aucun reset logiciel supplémentaire, seulement celui du flash.

`03-discovery-card.log` : WHO123=0x7B, CPU160MHz, stage99/error2,
last_cmd0/R1FF, initializedfalse/write_enabledfalse, writes0/CRCerrors0.
**L'absence d'erreur CRC n'est pas une validation CRC puisque la carte n'a pas répondu.**
Aucune mesure de FS16 ni calibration/gate d'enregistrement dans cette tentative.
Cause du silence SD non établie : état résiduel, contacts/câblage/alimentation ou autre
restent des hypothèses. La coupure froide est la prochaine vérification discriminante,
non une correction prouvée. État conservé : firmware découverte arrêté sur erreur,
aucune écritureSD en vol, GDB détaché ; pas d'image longue prête en Flash.

## Powerbank, limites et suite immédiate

Utilisateur : sans marque22000mAh, utilisation envisagéeCN15. Procédure préparée dans
[TR2_001_AUTONOME.md](../../../experiments/r3_durable_fat32/TR2_001_AUTONOME.md),
références primaires ST UM2861/BSP. PositionJP6/révision réelle non confirmée,
aucun jumper déplacé et aucun essai powerbank réalisé. Vérifier la sérigraphie avant
instruction physique CN15/JP6[7–8] ; ne pas utiliser CN1chargeur avec un hôtePC.
Ni capacité huit heures ni maintien en faible charge ne peuvent être certifiés par
la seule indication22000mAh ou un essai60s. N8G resteFAILED et sa cause non résolue ;
aucune immunité de001 ni endurance8h de la chaîne partagée revendiquée.

Ordre de reprise, sans refaire les tests acquis :

1. Attendre la confirmation de coupure/remise sous tension de001 déjà demandée ;
   relire le résultat du firmware découverte sans flash/reset supplémentaire.
2. Si CID/CSD accessibles, relever capacité/MBR/BPB, lier uniquement cette512Go,
   monterFAT32 et contrôler libre/nom/LBA ; sinon conserver le diagnostic et préciser
   la vérification de liaison électrique nécessaire, sans changer de carte par défaut.
3. Build/flash/readback de l'image courte liée, WHO/FS16/calibration puis gate3s ;
   qualifier USER/LED, éventuellement60s powerbank sans ST-LINK, relecture nativePC
   complèteCRC/temps/qualité/SHA. Tout transfert de carte/câblage reste utilisateur.
4. Image longue à préfixe neuf, retourWAIT réel et runbook final ; **départ long par
   l'utilisateur uniquement** après installation. Publier résultats et relectureGitHub.

## Publication et journal

Ce checkpoint IN_PROGRESS conserve les acquis, il ne clôture pas la qualification.
Journal `ETAT_COURANT_TR2.md` actualisé avec préparé/observé/non vérifié et prochaine
action physique ; ancienne synthèse N8G conservée intégralement. Preuves légères dans
`preuves/TR2-20261101-001-AUTONOME`, originaux locaux `/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101`.
Publication par push normal expliciteHEAD:main puis comparaison intégrale des fichiers
récupérés de GitHub ; le SHA du rapport et la preuve de relecture seront consignés après
publication. Aucun accèsCOM4, écritureFRAM, opération002 ou modification de protection.
