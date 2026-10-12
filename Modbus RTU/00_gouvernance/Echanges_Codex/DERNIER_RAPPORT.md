---
mission_id: TR2-20261012-M014-H-PRODUCTION-RESUME
status: BLOCKED
base_ref: main
base_sha: 30f4663c7eece9ffb66740da1466c0ae88432f5a
initial_head: b3a4ac7125b59f57979abd0a1458c11e52515504
result_sha: 0b2a901e185c9cbf8c02410bf2a29a0a3a75491e
created_at_utc: 2026-10-12T08:36:01.061944+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-H — Résultat final technique, publication en cours

**Les critères physiques M014 B0/B1, polling et perte/reprise sont satisfaits dans la portée ci-dessous.** BLOCKED porte uniquement sur la publication/relecture finale encore à obtenir au moment de ce commit. Aucune approbation intermédiaire ni intervention physique attendue. Ce résultat ne qualifie ni l'acquisition ni le stockage ni le produit industriel.

## Lots terminés et preuves

| Lot | Résultat établi |
|---|---|
| Reprise G | HEAD initial b3a4ac7, arbre propre, deux stashes conservés ; archives G exactes depuis30f4663 ; 113 firmware et539 .NET G non relancés sur code inchangé |
| Composition | Strong binding réel STM32, contextes durables FRAM SPI1/CS PD14, IIS3DWB SPI3/CS PC9, transport LPUART1/PG4 qualifié ; branche production avant tout harness ; SD complètement désactivée |
| Identité | UID96 `004A003D 38425018 20333157`, DeviceId32=1, unit1, TR2-000001/MSM, hwprofil1/fw0.14.8/proto1/caps0xC issus du registre/manifeste ; aucun UID dérivé ni B0 fixture |
| Provisioning | Unique trigger explicite, FRAM10→11(config slot1)→12(V2 identité), readback262144 complet exact à simulation ; reset et ancienplan refusé sans retry |
| Supervision | Campagne corrigée réelle B0/B1, polling3+3, timeout, invalidation puis réouverture série/B0neuf/B1frais ; compteurs indépendants RX72/TC8/erreurs0 |
| Profil permanent | Image sans observer/silence, flash/verify/readback/boot réussis ; ultime contrôle physique2FC03 B0/B1 réussi sur ce BIN exact |

**Configuration structurellement valide, activation opérationnelle différée.** V1 exige storage_mode1/limitepositive ; la configuration stockée porte1Mio. Le contexte offboot contrôle uniquement les contraintes structurelles avec un plafondUINT32_MAX explicitement non présenté comme capacité utilisable. Aucun stockage n'est attribué. Boot normal environment=false/capacité0 → ConfigurationRecovery UNAVAILABLE, aucun snapshot actif. READY et STORAGE_AVAILABLE restent0. Port campagnes indisponible, aucune substitution par backend FRAM. L'arbitrage ChatGPT43f37db autorise cette portée dégradée pour M014 ; il ne qualifie pas bulkSD. Pas de changement du layoutV2 ni des gelsV1.

## Provisioning et firmware évalués

Cible ST-LINK `003500463235510B37333439`, NUCLEO-U575ZI-Q, STM32U575/U585 ID0x482/RevU/3.29V. UID réel recontrôlé. BackupFlash2Mio G conservé et nouveau dump H comparé à l'image probe courante ; RAM768K préservée avant resets/flashs pertinents. BackupFRAM262144 double lecture identique puis extraction complète byte-equal au backup G avant trigger, SHA`59d32d12ebe26db36d8552ae522b2704632c6ccfde9e1a57200c3d25c77b4737`. DiagnosticE BIN/ELF et rollback de diagnostic conservés ; downgradeproductionV1 après V2 interdit.

Manufacturing source82169af puis réappliquée byte-identical/fa7b464 ; BIN67028 SHA`92df5bd857156cec58d0c375d3fd02a51f9d6b5f7f000a7c65750b0d493e1823`. Preflight readonly stage2/result0/gen10/trigger0. Un seul triggerRAM0x4D313448 : résultat0/gen12. Reset de vérification puis lecture complète identique à la simulation : SHA`d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce`, activeB/V2, autres domaines historiques préservés sauf slot1 config autorisé et identité96 ajoutée. Plan10 périmé ensuite refusé INVALID_STATE/trigger0 sans write. Identitérecordgeneration1 distincte de générationimage12.

Firmware corrigé réellement évalué : `0b2a901e185c9cbf8c02410bf2a29a0a3a75491e`. Imagequalification143828 SHA`089bff0854421c67f4248882470d4b178a1372ab58a074a8989fc9b04a6a5c56`, observerON/silenceON. **Image actuellement laissée sur la carte** : production permanente143108 octets, SHA`b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611`, ELF`c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de`, observerOFF/silenceOFF/TXqualifiedON. Pas de symbole SD read/write, manufacturing trigger ou observer dans l'ELF permanent. BIN relu byte-à-byte identique, bootV2/gen12 et B0 persisté conformes. Le profil permanent a aussi son propre essai physique2lectures ; pas succès extrapolé du seul flash.

Flashs : CubeProgrammer NORMAL/ST-LINK attendu/0x08000000/-w/-v/-rst, effacement des seuls secteurs nécessaires (manufacturing0..8, productions0..17). Jamais HOTPLUG, mass erase, Option Bytes, RDP ou changement de câblage. Chaque CLI suivi de resetGDB sûr avant trafic. Aucun GDB pendant échanges ; relevés APRÈS sans reset, puis MCU détaché/enexécution/serveurs terminés. Permissions exercées : commit/pushnormal, flash standard/debug qualifiés, provisioningFRAM explicitement borné et essais H ; aucun accord consommé C/I rejoué.

## Validation, corrections et efficacité

- Nouveauxprofils manufacturing/production/default cross-build réussis ; six tests du générateur réussis. Répétition offligne **sur copie du backup réel**, refus d'absencepermission/UIDincorrect, générations10→11→12, six writes, conservation du préfixe, B0attendu et configUNAVAILABLE sans ressource.
- Première campagnephysique9requêtes/8réponses : polling/perte/reprise acquis, mais uptimeB1figé1s. Journal complet SHA`2E63B72CA75184C7866F52184AE50721748A7B979624B9B36C47EAFD45150DC5`. **Non utilisé seul pour clôturer M014.** Cause : copie de projection boot sans producteurstatusvivant.
- Correctif minimal callbackrefresh_status opt-in fourni par production ; agrégationruntime avant réponse ; NULL préserve politique historique de publication et retrait explicite B1→04. Test négatif reproduit l'uptimefigé ; nouvelle assertion temps sans B5 puis testcorrigé réussi, assertionsanciennes inchangées.
- Premier essai d'actualisation inconditionnelle incompatible avec autorité de projection existante : échec conservé, corrigé par opt-in, sans affaiblir le test. Validation complète alors justifiée par cœur/interface partagée modifiés : depuis racine du worktree, `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, **113/113 + cross-build retour0** (`firmware-validation-b1-live-authority.log`). Quatre warnings newlib usuels, pas erreurs.
- PCsourceG1507dcbe inchangée,539tests conservés nonrelancés. PublishRelease win-x64 autonome réussi,371fichiers vérifiés surHP, OFFLINE réel retour0. Paquet identique réutilisé pour correction/final, sans rebuild supplémentaire. WrapperPS syntaxe analysée et OFFLINE contrôlé après évolutions ; aucun port dans OFFLINE.
- Erreurs mineures conservées/corrigées : dumpGDB UNC avecguillemets remplacé par cheminforward-slash qualifié ; includeConfigurationStore du checkhost ; comparaisonlogique initiale à imageA corrigée à activeB (les dumps physiques étaient déjà identiques).

## Essais physiques et journaux complets publiés

Les trois journaux bruts, analysescompactes, SHAartefacts et provenance des entrées évaluées sont dans `preuves/TR2-20261012-M014-H/`. Tous les journaux sont réellement récupérés parSCP puis hachés/comparés au hashHP ; contrairement au journalhistoriqueC, leur contenu intégral est vérifié et publié.

**Qualification corrigée**, source0b2a901 : HP08:18:53→08:22:24UTC, 9FC03 (B0/21 deuxfois, B1/20 septfois dontun timeout),8réponses, retry0, deuxsessions physiques série fermées. Polling3+3 à2s, uptime9/11/14 puis217/219/221s. LastValue/ReceivedAt antérieurs conservés avec IsAvailablefalse ; fermeture/invalidationUnidentified, réouverture réelle du driver, nouvelleidentité puis B1frais disponibles. B0seul ne réactive pas staleB1. Journal8487octets/31événements SHA`26213780794828107C67CCEF76180A60C81BD0EA45A5D008164C68AA0FD1B6E7`. MCU APRÈS : RXcallback/bytes72, TXstarted/TC8,10événements consommés dans silence,9T1.5/9T3.5, UARTerrors/drop/failsafe/pollerrors0, filevide, IRQ49/66actives, CFSR/HFSR0.

**Profil permanent final**, HP08:30:40UTC :2FC03 exactes0/21 et1000/20, retour0/connexionfermée/retry0, B0full byte-for-register identique attendu, B1statut1/flags0/uptime9/storage0. Journal1386octets/7événements SHA`DBCD4482F7CB2CF3CA089BE23D6D2362D4D025BB7B339C6526DF4ECF3136A996`. RelevéGDBAPRÈS sansreset : stage2/result0/pollerrors0, V2/gen12, configurationUNAVAILABLE, UARTerror0/TXidle/fileheadtail20/IRQactives/faultregs0. Les compteursobserver sont absents du profil permanent, pas fabriqués.

Total H :20requêtes physiques (9initiale +9corrigée +2profilfinal),18réponses et2timeouts planifiés, aucune écriture Modbus ni retry. Deux préflights refusés avant ouverture de port ne sont pas comptés comme trafic.

## Incidents d'environnement et Git

Main a évolué b3a4ac7→43f37db pendant tranche (arbitrageSD/communication). Push82169af refusé nonfastforward. EscaladeAGENTS§7 puis autorisation explicite utilisateur : nouvelletranche byte-identique dans worktreeisolé sur43f37db, commitfa7b464/pushnormal. Pas merge/rebase/reset/force ; dépôtoriginal et82169af préservés. Commits incrémentaux suivants39d1467(provisioning),0b2a901(correctionB1),b88a49f(planqualification distinct),f3d9a26(preuves corrigées et contrôlefinal). Relecturesdistantes intégrales intermédiaires15/5/8fichiers déjà prouvées ; publicationfinale à compléter.

PremiergateHP refusé pour différencehorloges avant ouvertureCOM7 ; mesureSSH readonly entre timestampshôte, conversion avec borneinférieure conservatrice, garde0..30s inchangée, horlogessystème nonmodifiées. BaselinesGDB uptime<15s/étatsains ; tousdebuggers fermés avant trafic.

SecondpaquetHP identique : extraction échouée disqueplein, aucunport. Réutilisation du paquetHinitial vérifié ; seule copieZIPtemporaire nouvellementcréée parCodex retirée aprèshashégal à l'original préservé (42.38→92.49Mo libres). Dossierpartiel laissé enplace, aucunfichierpréexistant/utilisateur supprimé. Aucune configurationWindows/SSH/Codex modifiée. Logsuniques/mutexCOM7/candidatsmaître/PnPFT232 etdeadlinephysique280s, contrôlefinal30s. PnP/mutex seuls ne prouvent pas exclusivité universelle ; ouvertureexclusive et échanges du driver sont observés dans ce banc.

## Limites et suite

M014 clôturable uniquement pour supervision réelle B0/B1 et perte/reprise de disponibilité du répondant. La perte est une fenêtre firmware90..180s sur le vrai bus, **pas retrait de câble ni coupure d'alimentation** ; reconnexionphysique = fermeture/réouverture réelle du driversérie. Pas de capture brute filaire, mesureDE/TC/dernierstopbit/t1.5/t3.5/contension, endurance/CEM/resetindustriel. M013 garde ses preuveshistoriques bornées et réserves ; sa qualificationgénérale n'est pas affirmée.

MicroSD totalement désactivée, zéroallocation/zérolecture/zéroécritureH ; partitiontype07/LBA65536 inchangée par construction des profils sansaccèsSD, pas relecture inutile de tous ses secteurs. Acquisition/vibrations/bulkSD/activationopérationnelledeconfiguration/E4 nonqualifiés. Certains champs B1CPU/mémoire/température/usage restent neutres fauteproducteurs, pas mesures physiques. Pas qualificationWeb/SQLite intégrée par cette CLI ; suite roadmapM015 à contractualiser pour acquisition réelle, stockage traité séparément avant activation. AncienjournalHP C reste nonvérifié à partir de son SHAannoncé.

Journalactualisé (étatFRAM/production/essais/réserves) et roadmapmiseàjour (M013borné, M014clôturetechnique, M015proposé). RevueindépendanteChatGPT attendue, pas présumée.

## État final et preuves de reprise

Preuves complètes locales : `/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z` ; BIN/ELF/backups/scripts/stdoutGDB/CLI/HP/logsvalidation conservés horsGit, petitsjournaux et analyses publiés danspreuves. Manifesteplan `COMMISSIONING_M014_H.json` conservé comme entrée de build ; résultatcourant distinct `COMMISSIONING_M014_H_RESULT.json`. Hash des entrées avant actualisation du registre dansevaluated-inputs.json. RegistreASSIGNED désormais PROVISIONED_V2/gen12.

Worktreepublication `/home/lolo/dev/msm/projects/MSM_vibe-M014-H` sur branchelocale codex/m014-h-integration, destinationautorisée explicitemain. Dépôtoriginal `/home/lolo/dev/msm/projects/MSM_vibe` surmainlocal82169af préservé et volontairement nonréécrit ; ne pas y faire de pull/rebase automatique. Deuxstashes005/002 inchangés. État propre à confirmer après commit/publication dans lesdeuxdépôts. Aucunbloquagematérielactuel ; qualificationindustrielle et nouvellemission hors de cette clôture. SHAfinaldu rapport communiqué après push/relecture, sans référence circulaire.
