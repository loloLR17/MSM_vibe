---
mission_id: TR2-20261019-DEMO-METROLOGY-UX
status: DONE
base_ref: main
base_sha: cc7c9632f01a01a42442cb7c752ff7b3bcc3176e
initial_head: 69e18c23c39aa642e6c0e6db530288cf53dddd9b
result_sha: f8dbfefdd89827069acad4bdae7ebdeef7426689
created_at_utc: 2026-10-28T21:57:58.701707+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Accélération comprise, écrans LIVE refondus et déployés sans remplacer le firmware ni les exécutables

**P1-A et P1-B livrés et relus sur GitHub.** L’utilisateur confirme que la date passée du20/10 ne change pas l’objectif. La chaîne testée n’a pas de plafond2mg : le calibre est **±2g**, la conversion enmg est correcte, et les indicateurs B3 absolus conservent une forte composante constante compatible avec la pesanteur. Le démarrage calibre le **temps FIFO**, pas l’accélération. Aucune compensation ni évolution normative appliquée. La présentation est réellement déployée sur le service Windows fonctionnel, testée dans Edge sur données physiques. P2Web est documentée et différée conformément au contrat ; les lanceurs START/STOP restent utilisables.

**Dernier état observé à21:54:57UTC : TR2-001 connecté, STOP sans mesure valide / séquence0, service Windows d677/63f3 inchangé actif localhost5100/COM4unique, GDB détaché. DernierSTOPdurable observé5/25 ; firmwareLIVEc36 vérifié21:18, zéroflash/reset de Codex. La transition tardive correspond à une perte d’alimentation accidentelle ensuite confirmée par l’utilisateur ; ses observations et cette confirmation sont distinguées ci-dessous.** URL : `http://127.0.0.1:5100/live-fleet.html` et `/live-device.html?deviceId=1&view=vibration`. Recharger la page pour prendre les assets actualisés. STOPreste une commande lente (~40s), pas un arrêt de sécurité industriel.

## 1. Référence réelle et préservation

Fetchmain69e18c23, AGENTS/gouvernanceA06/protocole/contrat et journal intégral consultés, archivesCOM4 et conceptionUX relues. Nouveau worktree `MSM_vibe-demo-metrology-ux`, branche `codex/demo-metrology-ux`, fondé sur le main distant. Le main historique82169af propre, **73autresworktrees et deuxstashes** restent préservés ; aucun merge/rebase/reset/clean. L’ancien rapportCOM4 est identique à son archive avant remplacement.

Au départ aucun service/debugger Windows actif. Le premier inventaireWMI ne montrait pasCOM4 ; **c’était une limite d’inventaire**, corrigée par PnP FTDI0403/6001/BG04KSSW, GetPortNames et vraie réponseB0/B3 ; la demande de reconnexion a été retirée en commentaire. Confirmation utilisateur de son raccordement reçue, aucun geste physique supplémentaire nécessaire. Le premier lancement readonly s’est fermé après une observation arrêtée ; journal conservé, cause précise non attribuée. Relancement normal stable, sans reset/flash. L’état initial réel est STOP sans valeur valide, pas le HELD19/10 supposé.

Les377fichiers initiaux de l’application égalent le manifeste qualifié précédent. Snapshot natifSQLiteWindows initial **24337lignes/intégritéok**, source ouverte readonly ; aucun transfert d’une ancienne base ou d’un ancien ledger. Le ledger initial indiquait23 : nouvelles clefs24/25 allouées par le contrôleur éprouvé après gardesB0/B3/B5, pause tokenisée et exclusivité. Aucune clef5/12..23 réutilisée, aucunRENEW ni reprovisionnement.

## 2. P1-A — faits physiques et sens des mg

Lecture du chemin **réellement exécuté** c36 : `stm32_r4u_runtime.c` → `stm32_r4s_bench.c`/`s_calibrate`, `live_consume`/providerFIFO, `live_metrology.c`, `iis3dwb_units.h`, `window_statistics.c`, `indicator_calculator.c`, projectionB3 et JS/SQLite. Le vieux driver polling `stm32_iis3dwb_vibration_source.c` a aussi été inspecté, sans le confondre avec le cheminFIFO actif.

|Preuve|Résultat observé / portée|
|---|---|
|Deux fenêtres historiques4096 réanalysées horsligne|Conversion exacte aux anciens résultats962mg ; aucun rail. Pas d’ancien essai physique répété.|
|Nouvelle fenêtre4096 du run17|RMS G/X/Y/Z **954/950/88/12mg**, crêtes **1002/996/126/43mg**, concordance exacte au premierB3seq1 reçu surCOM4 par le contrôleur.|
|Bruts de cette fenêtre|X−16333..−14964, Y−2071..−786, Z−712..648LSB ; zéro−32768/+32767. Pas de saturation démontrée.|
|Composante constante|MoyenneXYZ−950,281/−87,388/+2,639mg ; norme954,294mg. Gravité dominante compatible, pas un étalonnage physique certifié.|
|Diagnostic centré **horsligne uniquement**|RMS global19,391mg après retrait de la moyenne par axe ; ni valeur B3 ni mesure vibratoire calibrée.|
|FS / configuration|Active3/gen3, code0,26667Hz,N4096,P2000ms,axes7. `fifo.full_scale=0`, WHO7B/CTRL1_XL00 relus aprèsSTOP. Les gardes physiques inchangées vérifient le FS au START/poll ; pas de halt pendantFIFO.|
|Mesures du service durant ce run|464observationsV/F, RMSglobal952–959mg, soit variation7mg ; aucune limite2mg. Anciennes observationsV/F951–2100mg dans la base initiale.|

Le RMS est `sqrt(mean(x²+y²+z²))`, sans retraitDC. La crête est le maximum de la norme du vecteur ; les axes sont traités séparément. Le codeconvertit les bruts avec61/122/244/488µg/LSB pour0/1/2/3=±2/4/8/16g, puis arrondi entiermg. `s_calibrate` ne calcule aucun biais ni gravité : compteurcapteur/DWT et résidu temporel seulement. Une orientation redistribueXYZ mais n’explique pas à elle seule l’écart de norme954–962mg à1g ; aucune normalisation à1000mg ou cause de biais inventée.

**Le constat utilisateur exact «2mg» n’est pas localisé rétrospectivement faute de capture/période originale.** Code et chaîne actuelle réfutent un plafond2mg pour les données évaluées ; une petite variation n’est pas la pleine échelle. Le graphe précédent était déjà0..max(points), sans clamp2mg. Ni augmentation deFS, ni modification deconversion/FRAM/identité/seuils justifiée. Proposition séparée d’indicateurAC versionné, bande/passante/fenêtre/crêtes/validité et étalonnage à définir ; décision structurante avant adoption. **Aucune soustraction de1000mg auRMS** ni modification des historiques.

Correspondances prouvées distinctement : **4096bruts → conversion → premierB3 physique**, puis **fenêtres ultérieures du même run → SQLite → API → DOM/CSV**. Le service était pausé durantSTART ; seq1 provient du contrôleur, n’est pas fabriquée dansSQLite. UTCfenêtre/run/epoch/config non exposés par le profilPC restent inconnus, sans attribution depuis un segmentPC.

## 3. P1-B — présentation professionnelle effective

Parc, sixonglets et observateur : `live-fleet.html`, `live-device.html`, `live.html`, nouveau `live-ui.css`, troisJS deprésentation/historique et observateur. ArchitectureS7/API/SQLite/autoritéinchangées. Identité réelle, affectation inconnue explicite, transport/acquisition/qualité/sévérité distincts, RMS/crête absolus enmg, STOP/HELD sans alarmeinventée. Une dernière valeur connue porte son propriétaire et sa réception ; **âgeB3 transmis distinct de l’âge deréceptionPC**, donc HELD ne paraît pas rajeuni par le polling.

KPIs puis courbeglobale visible à1080p ; typographie/espacement cohérents, diagnostics repliés après les courbes, contrôlesRMS/crête/XYZ, CSV/plages/filtres/tris conservés. AxeY gradué àorigine0 par défaut ; détailvariation àoriginenonnulle explicitement annoncé, sans transformation des nombres. B1/B2/B7/run/epoch/config indisponibles signalés. Les nodes decontrôle survivent au polling, focus et sections ouvertes conservés. Aucun boutonSTART/STOP fictif.

**Vrai Edgeheadless1920×1080** : captureavant, sixonglets/parc/observateur après, capturesRUNNING/HELD/perte logique. Largeur768px : fiche et parc sans débordement horizontal. DurantRUNNING,328→350points globaux sur les premières captures ; contrôlesXYZ804→536points en masquantX. Vérification finale :375observationshistoriques réelles,750pointsglobaux et1125→750XYZ, originevariation940..1020mg annoncée. Zéro exceptionJS ; focus et détails ouverts conservés. Les screenshotsRUNNING précèdent seulement le dernier ajustement d’âgeHELD, couvert ensuite par les capturesfinales/perte. HELD actuel avec une plage réellement acquise reste explicitement historique ; aucune archive substituée à unLIVEactuel. Contrôle et captures dans les preuves, pas une simple maquette.

Trois révisions d’assets ont passé le gate **publication complèteWindows COM99**, car des corrections de présentation étaient intervenues entreelles. Gatefinal `windows-held-age-gate` : **378fichiers,4HTTP200,exit0,SAC1,zéro3033/3077corrélé**, empreintes exactes àl’application finale. Aucun rebuild.NET ni échange deDLL ; touslesEXE/DLLrestent identiques aux377fichiers initiaux. Seuls8assetsWeb actualisés, backupbytes dans `TR2-DEMO-METROLOGY-UX\static-rollback`, manifeste et procédurede retour conservés. Le serveurphysique est resté inchangé pendant l’actualisationstatique ; fermeture uniquement pourl’auditGDB puis relancementnormal. Aucune protectionWindows/ADS/EA/politique/certificat modifiée, confiancefuture/signaturedurable nonprésumée.

## 4. Qualification bornée et état final

Un seul nouveau couple nécessaire à la qualificationdelaprésentation et àlamétrologie : **START5/24/run17** terminal4/0+V/F en40,0117535s ; **STOP5/25** terminal4/0+arrêtHELD/nonFRESH en40,059635s. Aucun forçage, retry ou émissiondepuis le navigateur. Le contrôleur/libérationtokenisée/exclusivité utilise les exécutables qualifiés inchangés.

Un contrôle de perte **logique**, aprèsSTOP et sous lockopérateurexclusif, demande la vraiefermeture duhandleCOM4 et attend le token`.released`. Le navigateur montre perte/dernièreconnue puis retourréel ; aucun débranchementcâble ou fauxsnapshot. Les757observationssanssnapshot de la nouvelle tranche sont **toutes** `Operator logical disconnect: serial handle closed; no target read.` : pausesSTART/STOP et essaiUI explicites, aucune pannebus déduite.

AuditGDB corrigé **21:18:06UTC**, servicefermé/FIFOinactive, identitéSN003500463235510B37333439/UID004A003D/38425018/20333157 gardées. Première sessionFlashreadonly exacte147024octets **9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981** ; seconde àl’entréelive_runtime_step, depth0/servicingfalse/main_busyfalse, lecturescapteur/RAM/FRAM seulement. Aucun callbackchronométré interrompu par unappelC ; aucun garde/counter/deadlineeffacé. **Zéroflash/reset**, aucune restaurationM016.

FRAM262144octets **3c56ea55bbe01cc31fdd71e13bcc5a5af510ab2c8205f893d6c2dad22473a2b5**, journal126/root139/epoch5/run17/config3gen3/STOP25. Préfixe122880 et queue16384 **exacts à la référence précédente97f43a**, sans rollback ; zoneLIVE évoluée par les commandesnormales, pas byte-identitéabusivedeFRAMentière. Work806/service1967/gap2363µs sous1000/2500/5000, deadline_missedfalse/FIFO0/stop0/UART0drop0erreur. Mesures observées, pas WCETindustriel.

Après détachement **sansreset**, service natifrelancé et **B0/B3physiques à21:23:32.732UTC** : transportconnecté, STOP/HELD, séquence78,955/992mg. ContrôleWindows21:25UTC : seulServiceactif, aucundebugger, COM4/SAC1/zérorefuscorrélé,378fichiers exacts au gatefinal. FirmwareLIVE et serviceopérationnels conservés ; aucuneacquisition laisséeactive.

## 5. Tests, données et livrables

Validationproportionnée, sorties effectives conservées :

- `node --test .../tests/live-metrology-presentation.test.cjs` : **5PASS**, dont âgeHELD distinct et pasdeclamp2mg.
- `node .../tests/live-trends.test.cjs <preuvesR5B2>` : **8PASS**, continuités/ruptures/CSV/filtre/tri sur captures historiques réellement marquées comme telles.
- `node .../tests/live-overview.test.cjs <preuvesR5B2>` : helpers validité/HELD/perte/propriétaire conservés.
- `node .../tests/live-ui.test.cjs .../wwwroot/live.js` : **7scénariosDOMsimuléPASS** ; cette preuve est distincte du vraiEdge décrit plushaut.
- ChecksJavaScript/diff, troisgatesWindows pour révisionsdistinctes, vraiechaîneCOM4/Edge, auditphysique4096 et assertionsarithmétiques indépendantes. Aucun testC/crossbuild/ancienneendurance répété surfirmwareinchangé ; aucun assemblage.NET modifié justifiant un rebuild.

**Snapshot de21:25:45UTC :29306lignes/intégritéok**, backupAPI natifSQLiteWindows sourceouverte readonly, pas rawcopyWAL. Les**24337lignesinitiales sonttoutesidentiques** ;4969nouvelles, dont464V/F et1705HELD. API comparée aux payloadsoriginaux. CSVréels500+500+375lignes comparés champàchamp àSQLite, conservantnulls/HELD/identité/source ; aucun zéro substituéàuneabsence. Base etjournaux Windowsoriginaux conservés, pas réécrits. Snapshot/publiables compressés avec SHA256bruts/stockés.

[Note métrologique et propositionAC/P2](../../Supervision%20TR2/LIVE_METROLOGY_UX.md), [repriseopérateur](../../Supervision%20TR2/operator-demo/REPRISE.md), [preuves/manifeste](preuves/TR2-20261019-DEMO-METROLOGY-UX/README.md). Local : `/home/lolo/dev/msm/reports/TR2-demo-metrology-ux/`. **JournalETAT_COURANT_TR2 actualisé** : sensphysique, interface, étatSTOP25, versionsconservées et limites. Gouvernance/contrat/specifications/archives/dettesA04 nonmodifiés.

CodeUI réellement évalué **f8dbfefdd89827069acad4bdae7ebdeef7426689** ; sourcefirmwarec36c9718a75e6b40c45707a482e8db836296cab2 ; contenuServiced67741f24569fc93b7e23a570745c4af59a05fde (constructionb031+patchhistorique), Service.dll63f3ddc1a91d99599a719bc6a013aa52810a9813a9d08c40233ce45b29cb85fa. Commits63b8be96(refonte),f8dbfefd(âgeHELD),d56e2638(précisionmétrologique). Permissionscommit/push/debug exercées ; allow_flashtrue nonutilisé.

**Limites et suite :** observationd’origine2mg nonlocalisée, pasd’étalonnagezéro/sensibilitéSI ou vibrationAC qualifiée. ProfilLiveDiagnosticpartiel/UTCfenêtreabsente ; aucunproduitR6/industriequalifié. CompensationDC et sécurité/autoritéWeb demandent décisions distinctes ; conception/detteP2publiée, aucuneactivationrisquée. Aucun blocageempêchantP1. Cette livraison demeure soumiseaucontrôleindépendantChatGPT.

Publication intermédiaire **30c6f8d421f920435f33903966b2cae70ba7fc8e** récupérée depuis GitHub et relue intégralement à **2026-10-28T21:53:47UTC** : **201livrables byte-identiques**,185preuves avec SHA256bruts/stockés vérifiés, snapshot distant29306lignes/intégritéok. Aucune source protégée/gouvernance/archives modifiée. Preuve [PUBLICATION.json](preuves/TR2-20261019-DEMO-METROLOGY-UX/PUBLICATION.json). Un premier contrôle lancé avant la fin du push a refusé l’écart de SHA ; il n’a jamais été présenté comme réussi. Le contrôle ci-dessus suit le succès réel du push.

## Contrôle tardif de clôture — état plus récent conservé honnêtement

À21:54:57UTC, le même service natifPID7920 répondHTTP200 avec un vraiB0/B3 : transportconnecté, **STOP/flags4097/Vfalse/Ffalse/HELDfalse/séquence0**. Ce n’est plus le HELDseq78 vérifié21:23 et21:25. Analyse readonly du journal existant : perte à**21:45:25.452UTC**, retour à**21:45:30.905UTC** avec séquence0. **Confirmation physique ultérieure de l’utilisateur : perte d’alimentation accidentelle du TR2 pendant cette interruption.** Ce témoignage concorde avec le retour à STOP/séquence0, sans constituer une mesure électrique indépendante ni une mesure de durée de coupure. La cause n’était pas connue lors du premier constat ; cette confirmation lève cette incertitude sans nouvel essai. Aucun flash/reset/commande de Codex entre les deux. Le premier commentaire qui évoquait aussi un nouveau processusService était erroné : comparaison avec le processus pré-audit ; inventaires21:25 et21:55 donnent le mêmePID7920. Rectification publiée immédiatement.

Le ledger est toujours25. Le contrôle tardif confirme378fichiers exacts au gatefinal, toutesDLL/EXEinchangées, SAC1/zérorefuscorrélé, serviceunique/aucunGDB. Preuves : [FINAL_AVAILABILITY.json](preuves/TR2-20261019-DEMO-METROLOGY-UX/FINAL_AVAILABILITY.json), `late-state-transitions.json`, extraitreadonly35observations compressé et son manifeste, `late-preservation.json`. Le snapshot29306lignes est antérieur à cette transition ; les757pertes logiques classifiées portent uniquement sur ce snapshot et ne prétendent pas expliquer cette interruption tardive. Le journalWindows vivant est conservé, aucune ligne historique effacée.

La carte répond et reste arrêtée, sans mesure nonvalide présentée comme actuelle. **Aucun nouveau START/reset/audit après cette transition ; la coupure accidentelle confirmée ne justifie pas de répéter les validations déjà réussies.** DernierhashFRAM et Flash sont ceux de21:18, pas des relectures tardives inventées. La reprise en STOP sans auto-START est conforme au comportement autonome attendu après la coupure rapportée. Aucun défaut spontané de stabilité n’est déduit de cet événement. La livraison bornée métrologie/présentation et ses preuves acquises restent conservées. Firmware/service fonctionnels conservés pendant les actions de cette mission ; état actuel rapporté avec sa limite.

Journal et reprise opérateur mis à jour avec cet étatplusrécent. Gitpropre après le premierpush, mainhistorique/stashes/autresworktrees conservés. ClôtureDONE de livraison, distincte d’une qualificationindépendanteChatGPT ou d’une garantie d’endurance. Cette miseàjour est poussée normalement et relue également ; sonSHA et l’étatGit final sont fournis dans la réponse finale sans auto-référence du commit.

**Complément utilisateur après le premier commit de clôture5549a570 :** «Je te confirme une perte d’alimentation accidentelle du TR2 pendant la période d’interruption». Preuve [USER_POWER_LOSS_CONFIRMATION.json](preuves/TR2-20261019-DEMO-METROLOGY-UX/USER_POWER_LOSS_CONFIRMATION.json). Cause rapportée par l’utilisateur, cohérente avec les observations PC ; aucun nouvel essai, flash ou resetCodex.
