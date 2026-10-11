---
mission_id: TR2-20261011-M014-E-PRODUCTION-BOOT
status: BLOCKED
base_ref: main
base_sha: a3bd116511f985a92a9abf8b64479b186f34625e
initial_head: 5a8ba6ff918a6d872bd9137df0d66a8465304b4b
mission_sha: 065e445a9ba85138b3d4e4ce5fda25ddb7d9990f
result_sha: 00230c3334a3a9c83b4d89edf758c84a6b3fc1c3
created_at_utc: 2026-10-11T21:28:51Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# M014-E — socle logiciel validé, composition de production bloquée

**111/111 tests firmware et cross-build STM32 réussis ; six tests SSH réussis.**
Livrés : gouvernance SSH, audit V1/autorités, moteur de transaction de provisioning
avec gardes explicites, probe de boot en lecture seule et correction d'une ancienne
autorité conservée après recovery en échec. **BLOCKED** pour le choix d'allocation/
format d'identité et la composition de carte ; **M014 reste IN PROGRESS**.
Aucun flash/debug/reset, FRAM/SD physique, COM7/RS485 ou commande distante HP.

## Références et périmètre

Main initial 5a8ba6f propre, HEAD=origin/main, stashes 005/002 conservés. Fetch puis
pull --ff-only sûr vers065e445, seulement mission/architecture changées. Base a3bd116
vérifiée dans l'historique ; pas supposition de parent immédiat. AGENTS et protocole
inchangés/repris, mission lue, journal entier consulté, roadmap/rapport SSH DONE/
rapport D archivé/décisions approuvées/document D et sources/tests pertinents inspectés.
SSH précédent effectivement terminé/publication des 11 fichiers relus auSHA 5a8ba6f avantE.
Archive SSH créée octet pour octet depuis5a8ba6f (MISSION/RAPPORT immuables) ;
archivesD précédentes conservées, contratactifChatGPT inchangé. Aucun gelV1 modifié.

Code évalué : `00230c3334a3a9c83b4d89edf758c84a6b3fc1c3`, commit
`Add guarded production provisioning transactions and revoke stale recovery authority`.
Développement limité aux garde-fous/provisioning/média et au préparateurSTM32.
Les tranches logicielles indépendantes sont terminées ; arrêt sur décision
structurante explicitement prévue au contrat, pas sur quota ou économie de tests.

## Lot1 — gouvernance SSH publiée

README existant enrichi, sans duplication d'outil : topologieT1000 Windows/WSL Debian,
clientSSHWindows/alias/clé hôte/clé dédiée, HP Windows / PowerShell, permissions de
mission/rapportsGitHub/piloteunique, OFFLINE/logs/timeout/étatdistantinconnu,
mutex coopératif/COM7limites, sécuritéadmin/retrait. Architecture pointe désormais
sur rapport SSHarchivé stable plutôt que DERNIER_RAPPORT changeant.
Aucun secret/IP privée/clé/logvolumineux publié, aucune modificationSSH/service/
pare-feu/droits/configurationCodex/Windows. PassecondCodexHP.

PreuvesSSH-HP-001 conservées : HPnom attendu etPowerShell 5.1, retours 0/17,
paquetA370hashes et OFFLINEretour 0 sansport. **Historiques, non répétés enE**.
ExclusivitéCOM7 pas prouvée par PnP/listeprocessus/mutex ; compte administrateur et
PasswordAuthenticationglobalyes restent réserves. Aucun trafic physique qualifié
par SSH-HP-001. Six tests Python relancés enE, aucune exécutionHP supplémentaire.

## Lots2/3 — audit indépendant des sources et résultatsD

[PROVISIONING_PRODUCTION_M014_E.md](../PROVISIONING_PRODUCTION_M014_E.md) livre
matrice desautorités/états, sources normatives, options et reprise détaillée.
RevueCodex technique deD confrontée au code, distincte du contrôlehumainChatGPT :

- B0gelé0..20 contient DeviceId 32 bits MSW/LSW unique/persistant ; adresse n'est pas
  un registre B0 normatif, UID 96 bits ne peut être tronqué versDeviceId. B1statusUNKNOWN=0/
  NOMINAL=1/DEGRADED=2/FAULT=3, READY bit 0 des flags et snapshots réels, pas valeurs inventées.
  HALlocaleCubeU5 fournit trois motsUID ; aucun UID matériel lu enE.
- Image logiqueFRAM de 51 818 octets entièrement allouée : BootIntent offset 51 802 + taille 16.
  Identitéfonctionnelle/UID/adresse ne disposent d'aucune réserve approuvée.
  IdentityService est enRAM, pas source persistante de carte. Le profiltransactionnel
  qualification262144 physique n'est pas une allocation production implicitementGO.
- TransactionalMedia récupère EMPTY/VALID/UNSUPPORTED/CORRUPTED/UNAVAILABLE.
  Pas enumRECOVERABLE ; publication survivanteCRCvalide → VALID, sans réparation
  automatique. Absence d'interface ne démontre pas absence physique du composant.
- ProductionApplication/SystemRuntime/ModbusSystemServer et bulkSD sont développés,
  mais contexts/allocation/configmetadata/identité/adresse réelles ne sont pas fournis.
  Bootcomplet peut écrire journaux/BootIntent ; probe livré est seulementlecture.
  AbsenceSD ne donne pas automatiquement un runtime dégradé entièrement disponible.
  Le binding faible NOT_AVAILABLE reste intact, diagnosticErestaurable conservé.

Choix proposés, non imposés : domaineidentitéFRAM séparé avec lienatomique,
évolutionlayoutversion avec migration, ou réaffectation historiqueCampaignData
après migration/ownership. Risques des coupures entre domaines/donnéesharness et
allocationsSD explicités. SchémaUID 96 bits/DeviceId 32 bits/ASCII/versions/capacités/address
interne et manifeste de provisioning proposés ; pas nouveau registre ni format
persistant de carte créé. Attributionunique/version/adresse ne sont pas inventées.

## Code livré et limites exactes

- `production_provisioning.h/.c` : probe enlecture sanswrite ; plan avec génération/
  ownership/image51818 indépendante/immutable et **callback autoritatif inspectant
  tout le layout et l'identité persistée**. Sans callbackréel/ownership→refus.
  UID 96 bits exact/version/capacités/adresse1..247 puis projectionB0existante vérifiés.
  Identité/B0/générationfonctionnelle/adresse doivent rester identiques à l'image
  actuelle ; migration/changementadresse non implicitement autorisés.
- Applyexplicitement autorisé seulement, jamaisappeléparboot : replan, transaction
  uniqueviaPersistentStorageCore/TransactionalImageMedia, recovery+relecture51818,
  aucun retry. Identique et générationcouranteconforme→zérowrite/commit ; ancienne
  générationpérimée refusée. Flagsoftwarewrite_authorized n'est pas autorisation
  humaine/matérielle ni une protection d'accès système.
- **EMPTY toujoursrefusé** : pas créationinitiale, pasformat, pas codecstockageidentité
  ni parseurproduction fourni fauteallocation. Le moteur n'est pas prétendu
  résoudre le provisioning initial ou raccorder déjà B0/B1 de la carte.
- `stm32_production_runtime_prepare` utilise probe ; conserveVALIDrequis, transmet
  erreurs plus précises avant capteur/runtime. Fonctionfaible de carte inchangée.
- `transactional_image_media_recover` révoque recovered/recovery_required avant
  chaque tentative, puis seuleVALID restaureautorité. Anciencode laissaitinterface
  utilisable après une nouvellepanne/corruption : défaut reproduit et corrigé.
- Nouveau test survrai média transactionnel avec FRAMsimuléeRAM ; fixtureidentité
  native au début de l'image et identitéHostunit23/protocol7/DeviceId76543210,
  UIDtroismots explicites. **Pas codec durable/allocation carte**, jamaisoverride
  du binding STM32. Aucun backend fictif ajouté dans production, pasSDcommeFRAM.
  identity_available indique sourceB0 duplan, pas serveuractivé/B1/READY.

## Validations réellement observées

Preuves `/home/lolo/dev/msm/reports/TR2-mission014-E-20261011T212010Z` horsGit.

- CMake/buildciblé+CTestproduction_provisioning :1/1 réussi initialement, puis
  cas renforcés inclus dans suite complète. Log ciblé initial visible sortieoutil ;
  validationfinale et LastTest.log archivés, pas déclarés simples testspréparés.
- Contrôle négatif : nouveau test lié à transactional_image_media.c duHEADmission
  préfix → abortattendu(-6), assertionrecovered/récupérationpanne en échec.
  `negative-control.log` et sourcepréfix conservés ; ce défaut est logiciel/hôte,
  pas une nouvelle observationFRAMphysique. Mêmecontrôle dans suite corrigée passe.
- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`
  retour 0 : **111/111 CTest (110anciens +1nouvelexécutable)** et cross-build ARM
  réussis. Nouveaucore/preparateurSTM32 effectivement compilés (sortiebuild).
  Tests : absenceinterface/médiaEMPTY, VALID, paircorrompusurvivant, I/Oinaccessible,
  corruption/unsupported, révocationread/write/commit, UIDtroisièmemot/mismatch,
  version/adresse0et248/capacitésréservées/DeviceIdchangé, ownership/parserabsents,
  générationpérimée/imagealias, writeauthorizationfalse, planzérowrite, idempotence,
  commit/relecture/rebootRAM et coupurespartielles aux3phasespayload/header/superblock.
  Suite inclut tests existants runtime/bulk/configfault/RTUB0B1/disponibilité ;
  aucun test supprimé/affaibli, aucune attente artificielle changée.
- Quatrewarningsnewlibpréexistants `_close/_lseek/_read/_write`, paserreur/Werrorhôte.
  Text157876/data124/bss129080 ; BIN/ELF/MAP ethashes dansbuild-artifacts.json,
  code 00230c3 évalué. Compilation n'est pas flash/exécutioncible.
- `python3 -m unittest discover -s 00_Environnement_developpement/T1000/ssh-hp -v` :
  **6/6**, ssh-unit-tests.log. Documentation/codeSSH inchangéhorsREADME.
- Validationcomplètefirmware justifiée : changement du média partagé et gardesboot.
  .NETsources/protocole inchangés → pasrelance524 tests D/publishpaquet inutile ;
  résultatsD conservés commehistoriques, non assimilés aux111 tests E.
- BIN/ELFdiagnosticE sur disque rehashés exacts dc6ac75f…/4c496149…,
  preserved-diagnostic.json. Pas nouvellelectureMCU, sauvegardes etpreuves intacts.
  Diff/index/whitespace/périmètre inspectés ; logs/artefacts/secrets horsGit.

## Blocage réel et point de reprise

AGENTS§8/contratE : décision structurante non couverte à documenter, poursuivre
travaux indépendants (réalisés ci-dessus). Pour finircomposition/provisioning :

1. Approuver domainepersistantidentité/format/version et liaisonatomique au layout
   existant, ownership desrégionsFRAM/SD et stratégieharness/migration/sauvegarde.
2. Fournir registre d'attributionDeviceId unique, identitéB0/authenticitéUID 96 bits,
   protocol/firmware/hardware/capacités/address réels, configuration/métadonnées.
3. Implémenter codec/inspecteur complet autoritatif, créationinitialeEMPTY et
   migrations approuvées séparées du boot ; tests des coupures entre domaines,
   idempotence et mismatch ; aucune fixture native copiée en production.
4. Composer contextsFRAM/SD/config/capteur/transportdurables exclusifs puis
   testsdépendancesabsentes avant imageproduction ; préserverdiagrestaurable.
5. Sousmissionmatérielle distincte seulement : sauvegardes/rélecture/procedure
   provisioning/flash/reset/persistance ; ensuite B0/B1HPbornés et polling/reprise
   avec journalexclusif/revueChatGPT/maîtreunique, aucun GDB pendanttrafic.

Pas de question intermédiaire surlogicielautorisé. Aucun quota épuisé, aucuncodeinachevé
volontairementabandonné ; parser/allocation/créationinitiale sont explicitement
bloqués parchoixstructurants, pas cachés derrière un fauxVALID/READY.
CommandesC/Iconsommées nonrejouées ; aucun accès COM7/SWDetécrituremédia physique.
JournalHPoriginalC toujoursnoncollecté (réservecontenu/hashintégral maintenue),
réserves DE/TC/stopbit/t1.5/t3.5/retourRX/robustesse/reset/watchdog/J1global/E4 ouvertes.
JournalETAT_COURANT actualisé avecpreuves/progrès/blocage/options/repriseE.
ÀChatGPT : revoirleslivrables/déciderallocationformat etautoritésréelles souscontrat.

## Git et publication

Commit/pushmain autorisés ; flash/debugfalse respectés, aucune opérationphysique.
Commitcode 00230c3 validé ci-dessus. ArchiveSSH exactes/gels/stashes 005/002préservés.
RapportBLOCKED etdocumentation prêtsàpublicationnormale HEAD:refs/heads/main puis
fetch/relectureUTF8intégrale/égalité deslivrables. Statut BLOCKED restera sur la
décisionallocation/format réelle, indépendamment de la réussite du push.
SHAfinaldistant et étatGit seront fournis après publication/relecture V2.
