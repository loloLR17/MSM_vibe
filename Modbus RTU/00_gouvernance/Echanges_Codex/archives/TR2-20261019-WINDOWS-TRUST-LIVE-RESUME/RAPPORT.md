---
mission_id: TR2-20261019-WINDOWS-TRUST-LIVE-RESUME
status: DONE
base_ref: main
base_sha: 0d0c395dc0cf5f68193d3bade6df8fcdc75ad092
initial_head: cb34b3f1dd19cc04be0235cd568f61fb343e850b
result_sha: 8a54a289dae2dad312cf2245c0db03fc76cc67ef
created_at_utc: 2026-10-19T13:56:53.944422+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# LIVE physique c36c971 — deux cycles qualifiés dans la portée autorisée

**Essais physiques réussis dans la portée limitée explicitement approuvée par l'utilisateur : paquet c36c971 accepté par Windows, protections inchangées.** Deux START/STOP terminaux avec effets, second run surveillé pendant **au moins 710.709s (11.85min)** entre terminal START et demande STOP volontaire, courbes réelles Edge, SQLite/API et FRAM corrélées. Firmware LIVE conservé, acquisition arrêtée à la fin, service et COM4 fermés, GDB détaché. La lecture finale du capteur constate WHO7B/CTRL1=00/FIFO_CTRL4=80 (bypass)/CTRL10=00 ; dernier contrôle Windows SAC1, aucun refus corrélé, COM4 libre sans transmission. Livrables et preuves publiés dans cc66d2ef1a4924d6b9827965da807e75fb3d2617 puis relus intégralement depuis GitHub ; **DONE dans la portée limitée décidée par l’utilisateur**, signature durable séparée et contrôle indépendant ChatGPT à suivre.

Le nouveau correctif de pause `8a54a28` est publié/testé mais **son binaire Windows reste refusé et n'est pas utilisé physiquement**. `result_sha` désigne les développements de cette mission. Le PC et le STM32 physiquement évalués proviennent tous deux de **c36c9718a75e6b40c45707a482e8db836296cab2**. Aucun mélange de DLL ni nouveau firmware improvisé.

## 1. Windows : observations et limite de confiance

Politique effective SAC **VerifiedAndReputableDesktop**, état registre1 ; activation3099 et politique **0283ac0f-fff1-49ae-ada1-8a933130cad6**. Refus anciens/nouveau rebuild3033/3077. Ancien R5-B2 et c36c971 : DLL Domain non signées, SHA différents malgré source inchangée, pas de Zone.Identifier. L'EA ESBCACHE de l'ancien fichier n'établit pas une autorisation ManagedInstaller. **Raison exacte de l'acceptation historique ou du changement de décision de c36c971 non démontrée.** Réputation/cache par fichier est une possibilité documentée, pas une cause prouvée ici.

Le paquet c36c971 **inchangé** passe maintenant le smoke natif Windows COM99 inexistant : exit0, quatre HTTP200 (fleet/overview/history/page), zéro blocage CI corrélé, SAC1,377 fichiers applicatifs exacts aux empreintes antérieures. Il a ensuite réellement exécuté les quatre commandes et les deux services COM4. Les événements Windows et les manifestes complets sont conservés.

La recompilation Service.dll **21a440e3ec32f1e974e6c89765749852c3d83e1d9d6d6b767ada53acb67a2d17** échoue82, quatre refus corrélés, HTTP indisponible sous la même politique. Session non administratrice ; CiTool liste refusée0x80070005 ; aucun certificat CodeSigning accessible dans les deux magasins My ni profil de signature disponible. La voie durable exige un canal/certificat d'éditeur légitime approuvé puis le contrôle de chaque publication. L'utilisateur a expressément reporté ce chantier et autorisé la seule démo c36c971.

Aucune politique/registre/ADS/EA/protection Windows modifié, aucun certificat auto-généré, déplacement vers un chemin de confiance, signature empruntée ou runtime alternatif. Un script PowerShell auxiliaire de contrôle sans transmission a été refusé par ExecutionPolicy **avant exécution** ; aucune exception/Bypass ni relance alternative de ce script. Les commandes physiques utilisent exclusivement l'outil natif déjà accepté. [Méthode de contrôle et sources Microsoft](../../Supervision%20TR2/operator-demo/WINDOWS_TRUST.md).

## 2. Identité et installation maîtrisées

Préflight13:34:22UTC : ST-LINK **003500463235510B37333439**, UID **004A003D/38425018/20333157**, FTDI BG04KSSW COM4/1152008E1 ; COM3 exclu. M016 Flash2Mio SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26 et FRAM262144o SHA2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84 exactes aux sauvegardes vérifiées. WHO7B/ODRoff/FIFObypass/TSoff. Sauvegardes réutilisées et récupérabilité M016 préservées.

Image LIVE **147024octets**, SHA **9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981** ; ELF1327744octets SHAba999ab4a8aa674a977ca8e3ab95469eba46c0d12ff2cdbf98c89bcfb1a1cbe3. Build et cross-build précédents réutilisés, sources firmware inchangées. Flash standard qualifié à0x08000000, vérification outil puis préfixe relu exact. Boot autonome stage7/error0, acquisition arrêtée, journal78/root91 ; **aucun replay/START au boot**.

Après premier STOP : reset logiciel qualifié, reboot autonome observé STOP, journal86/root99 conservés. Seconde acquisition démarrée par nouvelle commande normale, sans trigger GDB. Aucune restauration M016 sans nécessité : **LIVE demeure installé**. Aucun accès microSD, OB, mass erase, reprovisionnement/rollback FRAM ou changement de câblage.

## 3. START/STOP et exclusivité COM4 réellement observés

|Commande|Epoch/transaction|Temps demande→terminal+effet|Résultat|
|---|---:|---:|---|
| start |5/14|39.850 s|terminal4/0 + V/F|
| stop |5/15|39.857 s|terminal4/0 + arrêt/HELD/nonFRESH|
| start |5/16|39.779 s|terminal4/0 + V/F|
| stop |5/17|39.871 s|terminal4/0 + arrêt/HELD/nonFRESH|

À chaque commande : lock opérateur unique, token de pause frais, acquittement après fermeture du poller, ouverture COM4 exclusive puis gardes B0/B3/B5, ledger durable **avant submit**, deux FC10 canoniques, attente terminal EXACT epoch/tx/code et effet B3. Aucune commande forcée après doute, aucune réutilisation5/12–5/13 ou retry d'effet, aucun RENEW. Les traces série constatent **zéro recouvrement d'ADU service/opérateur** sur chacun des quatre intervalles opérateur. Le service readonly n'émet aucun FC10. Contrôle négatif13:52:32UTC : une seconde ouverture COM4, sans écriture, est refusée par UnauthorizedAccessException pendant que le service poursuit ses lectures ; exclusivité Windows effectivement observée. L'ouverture exclusive réussie est observée avant la première transmission de chaque commande.

**Limite c36c971 :** le fichier d'acquittement peut disparaître un cycle sur deux pendant la pause ; le handle série reste fermé tant que le token de pause est présent. Les quatre handoffs ont réussi sans exception ni conflit. Une absence d'acquittement ou un refus d'ouverture arrête l'outil avant transmission ; ne pas forcer/manipuler les fichiers pour continuer. Correction stable et test20polls publiés8a54, adoption Windows différée. Ces quatre succès ne prouvent pas l'absence de toute race future.

## 4. Chaîne réelle, durée et mesures

Service **Windows win-x64 autonome** dans `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\`, COM4 réel, propre base `data\live.db`, port5100, `--seconds 0`. Le Web n'ouvre aucun COM ; aucune archive WSL5094 substituée. Source partielle LiveDiagnostic protocole2/cap12, B4/B6 indisponibles honnêtement, sans UTC fenêtre/run/config inventés.

- **6060 observations** SQLite, intégrité `ok`, chacune exacte au payload du service ; **18468 ADU** service à CRC valide, dont B3 {'cycle1': 491, 'cycle2': 4126} ; champs B3 bruts corrélés intégralement au décodage PC et à SQLite.
- **4062 observations courantes**, **314 HELD** ; **1443 pertes logiques** dues aux pauses opérateur, snapshot absent, aucun rejet B3 ni perte série inattendue observée. Les 45 échecs HTTP du collecteur pendant l'arrêt volontaire du premier service pour audit/reboot sont conservés séparément, hors RUNNING supervisé. Les trous restent visibles ; pas d'interpolation ou fausse continuité entre segments.
- **467 captures API/SQLite** corrélées par observationId, heure de réception et valeurs. **2000 lignes CSV** exportées par le vrai navigateur et contrôlées contre les payloads SQLite, champs inconnus vides.
- Edge **headless réel**, DOM/API/PNG/CSV conservés ; première paire RUNNING102→168 points globaux et153→252XYZ, réceptions et séquences renouvelées ; HELD explicitement non courant. Capture prolongée et captures finales conservées. Pas de déclaration de revue humaine d'Edge.
- Second run : au moins **710.709s** entre terminal START et demande STOP ; il fonctionne au-delà de l'ancienne borne10min. Cette durée est une **preuve bornée** ; aucun arrêt automatique ajouté au firmware/service et aucune endurance illimitée revendiquée.

L'API overview peut ponctuellement déclasser une observation fraîche en non courante si sa réception arrive après l'instant `now` capturé avant une lecture SQLite asynchrone. Exemple conservé : observedAt13:46:16.2332208, receivedAt13:46:16.2347316UTC, isCurrent=false/age0. Lecture du code cohérente avec un classement conservateur, pas une perte COM démontrée. **Non corrigé dans le paquet autorisé.** Les alarmes240/sévérité3 sont les données réellement émises avec la configuration3 et ses seuils warning0 ; aucune calibration/métrologie ni nouvelle configuration normative ne sont qualifiées.

## 5. Coexistence et persistance

Audits GDB **uniquement acquisition arrêtée et service fermé**, aucun halt pendant FIFO. Budgets1000/2500/5000µs :

|Cycle|Max work µs|Max service µs|Max gap µs|Erreur FIFO|Erreur UART|
|---|---:|---:|---:|---:|---:|
|cycle1|806|1980|2220|0|0|
|cycle2|806|2057|2211|0|0|

Aucun deadline_missed, stop_error, événement IRQ perdu ou tx_fail_safe observé. Les erreurs/compteurs sont des observations du firmware, complétées par les CRC et la chaîne PC ; elles ne prouvent pas une perte globale zéro au-delà de la fenêtre qualifiée. Les premières4096XYZ de chaque cycle sont relues et leurs RMS/crêtes recalculées indépendamment avec arithmétique entière : correspondance exacte aux huit indicateurs de la première fenêtre physique.

FRAM complète après chaque cycle **exacte au modèle des métadonnées exécutant le cœur réel R2** ; effets physiques prouvés séparément par FIFO/B3/STOP. Préfixe historique122880 et queue16384octets byte-identiques au préflight ; seules écritures LIVE [122880,245760). Final **journal94/root107/epoch5/run13/config3gen3/STOP5/17**, SHA FRAM **e18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15**. Aucun rollback/replay ou remplacement par le modèle. Premier essai de génération de l'oracle a échoué sur une substitution erronée78→86 de l'assertion initiale ; source/log conservés, générateur corrigé pour l'état initial réel78, sans affaiblir le contrôle.

## 6. Validations, Git, publication et suite

Code pause8a54 : **239 tests Service réussis** ;350 tests hors Service et cross-build/image spéciale précédemment acquis réutilisés sur code inchangé. Pas de relance des validations M014/M016 historiques. Le gate Windows négatif sur rebuild et positif sur c36c971 sont distincts d'une compilation, d'un flash et de la présente qualification physique limitée.

État initial : worktree isolé basé sur main cb34b3f ; main historique82169af propre,71 autres worktrees dont TEST-IHM,2stashes et7994fichiers protégés vérifiés inchangés. Aucun reset Git/clean/merge/rebase/force-push. Branche auxiliaire `codex/windows-trust-live-resume`, destination explicite main. Commits :8a54a28 correction/contrôle de confiance ;87604af checkpoint physique,117livrables relus intégralement depuis GitHub. Publication détaillée **cc66d2ef1a4924d6b9827965da807e75fb3d2617** relue depuis origin/main : **191 livrables byte-identiques,179 preuves brutes/stockées vérifiées**, SQLite récupérée du blob distant intègre/6060observations. Worktree de mission propre après ce push, main historique et stashes conservés. Cette clôture documentaire sera aussi poussée normalement et relue ; son SHA sera fourni dans la réponse finale sans auto-référence.

[Preuves et manifestes](preuves/TR2-20261019-WINDOWS-TRUST-LIVE/README.md), [mode opératoire](../../Supervision%20TR2/operator-demo/README.md), [reprise](../../Supervision%20TR2/operator-demo/REPRISE.md). Journal ETAT_COURANT_TR2 actualisé : firmware installé, deux cycles, durées/budgets, FRAM finale et limites Windows/qualification. Permissions exercées : développement/test, commit/push main, flash/debug qualifiés. Aucun geste physique demandé/observé dans cette reprise.

**À transmettre à ChatGPT :** contrôler indépendamment ce résultat limité ; signature/déploiement des futures versions et adoption du correctif de pause restent chantier distinct décidé par l'utilisateur. Ne pas requalifier les missions archivées BLOCKED, ni annoncer produit LIVE complet/GO industriel. Guide `LIRE_AVANT_DEMO.txt` installé hors du sous-dossier app ; cinq lanceurs inchangés, contenu identique au Git après normalisation CRLF/LF documentée. Aucun binaire modifié. Pour la prochaine démo : conserver ledger/FRAM, lancer service puis Edge, START, STOP terminal avant arrêt service ; pas de compilation requise. En cas de refus Windows ou d'incertitude COM, arrêter avant commande et conserver les preuves.
