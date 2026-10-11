---
mission_id: TR2-20261011-M014-F-PERSISTENT-IDENTITY
status: BLOCKED
base_ref: main
base_sha: 24bb47bf17c8e36d1799227900de61d517e92597
initial_head: 24bb47bf17c8e36d1799227900de61d517e92597
mission_sha: 1b4b6ef55833d9e20708c2b2bcc745b01b504c04
result_sha: 2ce3a54ae5f5aff9492f8e68465d00921661762e
created_at_utc: 2026-10-11T21:49:53Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-F — logiciel validé, production bloquée sur les autorités réelles

**112/112 tests hôte et cross-build ARM réussis.** Layout V2, codec d’identité,
création EMPTY explicite et migration atomique V1→V2 livrés. Composition STM32
préparée avec identité persistée et comparaison du UID96 matériel entier.
**BLOCKED** : attribution de production et manifeste de médias/configuration réels
absents. Aucun firmware de production opérationnel ni polling/perte-reprise physique
démontrés ; **M014 demeure IN PROGRESS**. Aucun flash, debug, reset, SSH HP, COM7,
RS-485 ou écriture FRAM/SD physique pendant cette mission.

## Références et gouvernance

HEAD initial `24bb47bf17c8e36d1799227900de61d517e92597`, main propre, stashes 005/002
présents. Fetch puis pull --ff-only sûr vers `1b4b6ef55833d9e20708c2b2bcc745b01b504c04` ;
seul le contrat changeait. AGENTS/protocole/roadmap inchangés et repris ; contrat F,
journal courant intégral, rapport E, audit E, sources et tests pertinents inspectés.
Normes B0/B1 conservées ; architecture F appliquée sans second domaine physique.
Archive E MISSION/RAPPORT exacte depuis 24bb47b, sans écrasement. Contrat actif
ChatGPT inchangé, preuves et travaux antérieurs préservés.

Code évalué : `2ce3a54ae5f5aff9492f8e68465d00921661762e`. Commits logiciels :

- `a5e49dee30708f928257706de3c4dda4a9b3ab85` — Add versioned persistent identity and atomic V1 to V2 provisioning.
- `2ce3a54ae5f5aff9492f8e68465d00921661762e` — Strengthen repeated identity provisioning rejection tests.

Le second commit renforce uniquement les tests. Le firmware reste identique au
premier. Arrêt sur préconditions autoritatives manquantes, pas sur quota ni sur
l’architecture déjà approuvée ; les mécanismes indépendants ont été développés.

## Format et transactions

[Spécification complète, allocation et reprise](../IDENTITE_PERSISTANTE_M014_F.md).
Calcul des bornes consigné **avant codage** dans layout-before-code.md hors Git ;
audit détaillé complété par extraction des constantes des headers avec programme C.

Préfixe historique [0,51818) inchangé, notamment BootIntent [51802,51818) et toute
la réservation CampaignData, même avec bulkSD. Identité [51818,51914), 96 octets.
Deux zones physiques d’images inchangées : header64 + payload51914 = 51978 <57344,
marge5366 par zone ; fin122880 < FRAM262144. Espaces non alloués ici non présumés
libres sur la carte. Metadata bulkSD8192 et blocs alignés512 restent sur le backend
SD distinct. Static assertions protègent la jonction BootIntent/identité et la taille V2.

Codec TR2D/version1/longueur96/génération64/CRC32 IEEE, big-endian explicite sans
padding C ; UID96 entier, DeviceId uint32, génération fonctionnelle, versions,
capacités, ASCII serial16/fabricant8, adresse1..247 indépendante et réservés zéro.
Version codec1/layout2 distinctes de la version protocole B0, fournie explicitement.
Projection B0 gelée réutilisée, aucun registre ajouté ni UID tronqué. CRC protège
l’intégrité accidentelle ; il ne prouve ni authenticité ni unicité du parc.

Le lecteur V2 accepte V1/V2 sans migration au boot. L’API V1 du **code F** refuse
une publication incompatible plus récente dont le superblock est intègre.
**Les anciens binaires antérieurs à F n’ont pas cette protection** et peuvent
reprendre un ancien V1 survivant. Downgrade production interdit sans restauration
dédiée autorisée et sauvegarde complète. Le diagnostic E isolé ne monte pas la FRAM.

Le moteur de provisioning exige image indépendante et immuable, propriété exclusive,
génération attendue, UID/protocole conformes, autorisation d’écriture, attribution
et validation métier du préfixe. Sans callbacks autoritatifs : refus. Le vérificateur
de registre contrôle les doublons UID96/DeviceId et la correspondance exacte des
entrées fournies ; il n’attribue aucun ID et ne garantit pas l’autorité du registre.

Migration V1 : copie byte-identique des 51818 octets, ajout d’identité et publication
unique payload → header → superblock dans l’image inactive. Création EMPTY seulement
sur appel explicite, après recovery EMPTY et validation du préfixe fourni, sans
format_empty ni image zéro déclarée production. Readback/recovery complet, aucun
retry. V2 identique et génération courante : zéro écriture. Changement d’identité,
adresse ou génération d’identité, double provisioning différent et génération
transactionnelle périmée : refus.

Coupure avant publication de migration : V1 survivant ; publication complète : V2
complet. Une écriture complète retournant erreur reste un échec de l’appel, même si
reboot retrouve V2 : relire l’état, ne pas répéter automatiquement. Création initiale
interrompue peut laisser EMPTY ou records non publiés invalides ; aucun reformat
ou réparation automatique. Résultats démontrés sur RAM simulée, pas sur FRAM réelle.

L’inspecteur fourni valide son codec et le lien UID/protocole/B0. Il ne certifie pas
à lui seul tous les records métier du préfixe : validate_prefix et les services
existants conservent cette responsabilité. L’API bas niveau publish_v2 n’est ni
une autorisation physique ni un contrôle d’attribution ; utiliser le moteur contrôlé.

## Composition et blocage exact

Runtime STM32 : candidate51914, identité et B0 durables ; autorité de propriété et
protocole exigée, FRAM réelle et recoveryV2, HAL_GetUIDw0/w1/w2 entiers, inspection
et comparaison UID/protocole avant capteur/boot. Hook simplifié aux dépendances
et transport qualifié : aucune injection B0/adresse. Binding utilise exclusivement
le B0 et l’adresse persistés, puis les snapshots/readiness réels pour B1.
V1/EMPTY/invalid/inaccessible ne déclenchent aucune création ou migration.
Le boot complet existant peut écrire journaux/BootIntent ; seule la phase probe/
identité est en lecture seule. Aucun faux READY ni override de fixture.

Weak hook demeure NOT_AVAILABLE : **registre autoritatif UID96→DeviceId et valeurs
B0 réelles/versionnées, configuration et manifeste de médias exclusifs absents**.
FleetRegistry PC gère les sessions et doublons actifs ; ce n’est pas un registre
d’attribution des identifiants. UID réel non lu sur cible pendant F, aucune valeur
inventée. Ces autorités doivent alimenter validate_prefix et les dépendances
SPI/CS/capteur/backend SD/config metadata durables. SDMMC expose le support brut
entier : sa propriété et ses allocations doivent être établies. Aucune instance
du harness empruntée, aucune substitution FRAM à SD. Ce blocage ne remet pas en
cause le choix de layout approuvé ; il interdit seulement le GO de production.

## Validations et preuves

Dossier : `/home/lolo/dev/msm/reports/TR2-mission014-F-20261011T213911Z`.

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh
```

Retour0, **112/112 CTest et cross-build STM32**, sur le contenu du commit a5e49de.
Validation complète justifiée par évolution du cœur partagé/recovery/format.
Logs intermédiaires et final conservés ; nouvelles modifications et renforcements
ont motivé les relances. Aucun échec masqué, test supprimé ou affaibli.
Assertions actives, flags hôte -std=c11 -Wall -Wextra -Wpedantic -Werror, pas NDEBUG.

42 combinaisons de coupures : 3 phases ×7 tailles0/1/31/63/64/25957/51914 ×2 modes,
plus deux succès sans coupure. Conservation préfixe/BootIntent, reboot/readback,
CRC indépendant Pythonbinascii=0x0C190038, chaque octet altéré, version/réservés/
adresse/capacités/génération invalides malgré CRC réparé, UID mot3/protocole,
autorisation/attribution/préfixe/alias/stale generation/idempotence/doublons,
inaccessibilité et révocation, refus lecteurV1 aprèsV2 et commit normal V2 préservant
l’identité couverts. Les tests existants config/journal/runtime/RTUB0B1 passent aussi.

Pour le second commit de tests seulement : build ciblé de test_persistent_identity
puis `ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' -R '^persistent_identity$' --output-on-failure`
réussis, 1/1, avec refus de double provisioning d’adresse24, génération d’identité
changée et DeviceId changé, sans écriture supplémentaire. Firmware inchangé,
donc pas de cross-build supplémentaire inutile. final-targeted.log conservé.

ARM text160196/data124/bss129304. BIN160320 octets SHA256
`ff3d530fc7157cf87da2dda6c55521d7f7b1824edc69ab5d5c2cb56bfa2fa33e` ;
ELF343828 octets SHA256
`b77edf161184df80084e045e2db6b189dd0cb4aa25e8249f0841d1c08edd49d8`.
BIN/ELF/MAP/LastTest/manifests copiés hors Git, **non flashés**. Artefact cross-build
du harness par défaut, pas image production GO. Quatre warnings newlib habituels
_close/_lseek/_read/_write conservés ; aucune erreur C hôte/build/test.
.NET et SSH inchangés : 524 tests D et 6 tests SSH E conservés comme résultats
historiques, sans relance/rebuild/transfert ou nouvelle commande HP.

Diagnostic E rehashé sur disque :
BIN `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`,
ELF `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`.
Pas nouvelle relecture MCU ni sauvegarde FRAM/SD physique ; dernier état observé
reste C. Allowflash/debug true dans F **non exercés**. Succès logiciel et preuves
disque ne démontrent aucune disponibilité/READY sur cible.

## Limites et reprise

Arrêt réel : autorités d’attribution/valeurs et manifeste médias manquants.
Aucun flash de harness ni COM7/RS-485 tant que firmware et données réels ne fournissent
les blocs nécessaires. Publication distincte de qualification. C/I consommés,
non rejoués ; journal HP original C non fourni, contenu intégral/hash non vérifiés
à partir du seul SHA annoncé. Timing DE/TC/laststop/t1.5/t3.5/retourRX, robustesse,
endurance/J1global/E4 restent ouverts. M014 IN PROGRESS jusqu’au polling et à la
perte/reprise physiques démontrés.

1. Fournir registre revu UID96 entier→DeviceId uint32 unique, B0 réel, adresse,
   versions et origine/approbation ; configuration valide, géométrie et propriété
   exclusives FRAM/SD, metadata et manifeste de sauvegarde.
2. Raccorder les autorités/callbacks et strong hook aux vrais handles/contextes
   durables, séparés du harness ; tester dépendances absentes et construire une
   image production identifiée/restaurable. Pas nouvel arbitrage du layout F.
3. Sous procédure média explicite et préconditions qualifiées : identifier cible/
   ST-LINK, sauvegarder/hash FRAM262144/SD affectée et diagnostic E, plan sans
   écritures, création ou migration unique hors boot, recovery/readback/persistance.
   Pas format ni downgrade implicite, aucun HOTPLUG CubeProgrammer.
4. Ensuite flash standard qualifié et observations sur cible. Lorsque B0/B1 réels
   sont disponibles : nouveaux essais SSH HP bornés, maître exclusif et journaux
   hashés, sans GDB pendant trafic, puis polling/perte-reprise dans le cadre prévu.

Journal actualisé : format, mécanismes, résultats, risque de downgrade, dépendances,
blocage et suite. À transmettre à ChatGPT : vérifier publication et fournir les
autorités/manifeste, sans redécider l’architecture F. Permissions exercées :
logiciel/tests/cross/commit/push normal sur main. Stashes005/002 conservés.
Publication normale `git push origin HEAD:refs/heads/main` réussie au SHA
`0abbad9e33c438943c6660584a65768feae7e792`. Fetch puis relecture intégrale UTF-8 de **15 fichiers**
(14 livrables et contrat actif), comparaison octet pour octet aux fichiers locaux
réussie ; preuve remote-reread-1.json hors Git. HEAD=origin/main, arbre propre
après cette première publication, stashes005/002 conservés. Aucun fichier hors
périmètre, secret ou artefact de build inclus, diff/index contrôlés avant commit.
La présente consignation est publiée dans un commit documentaire final, puis
relue à son tour ; son SHA distant final est fourni dans la réponse Codex sans
référence circulaire dans ce fichier. **BLOCKED maintenu**, autorités et preuves
physiques toujours manquantes ; publication vérifiée ne vaut pas qualification.
