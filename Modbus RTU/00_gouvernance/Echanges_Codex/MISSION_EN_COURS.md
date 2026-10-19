---
mission_id: TR2-20261019-DEMO-METROLOGY-UX
status: READY
base_ref: main
base_sha: cc7c9632f01a01a42442cb7c752ff7b3bcc3176e
created_at_utc: 2026-10-19T17:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# MSM TR2 — Démonstration 20/10 : sens physique des mg et refonte de lisibilité LIVE

## Objectif utilisateur et priorités

L'utilisateur a personnellement réussi à utiliser le LIVE après la remise en service COM4. Avant sa démonstration du **20 octobre 2026**, il demande :
- **P1 Données** : « accélération plafonnée à 2 mg », demande d'augmenter la plage si effectivement nécessaire ; comprendre si une **calibration au démarrage supprime la pesanteur**. Ne pas accepter comme diagnostic l'hypothèse d'un capteur limité sans mesure.
- **P1 Mise en page** : écrans actuels trop bruts et diagnostics trop dominants ; rendre l'ensemble de la supervision plus lisible, immédiatement compréhensible et visuellement professionnel, priorité aux indicateurs de supervision et aux courbes, sur **tous** les écrans LIVE.
- **P2 Actions** : intégrer START/STOP dans la supervision Web, plutôt que dépendre indéfiniment des .cmd. P2 ne doit pas empêcher une livraison P1 utilisable demain ; si l'intégration Web n'est pas entièrement qualifiable à temps, **la conserver comme tâche documentée séparée et ne pas présenter des boutons factices**.

Référence unique : GitHub `main` courant ; lire `AGENTS.md`, gouvernance A-06, protocole, mission, rapport COM4 archivé et `CONCEPTION_SUPERVISION_LIVE_UX_20261018.md`. Préserver architecture, protocoles gelés, B5/ledger, FRAM, matériel et validations R5/UX-L1/L2. La précédente mission COM4 est **DONE** et archivée sans requalification. Déploiement Windows Smart App Control/Code Integrity reste une contrainte réelle ; aucune confiance automatique pour des DLL rebuildées.

## P1-A — Audit physique et sémantique des accélérations, avant modification

1. Inspecter sources **exécutées** et preuves : profil `TR2_STM32_LIVE_OPERATOR`, `stm32_r4u_runtime.c`, `stm32_r4s_bench.c`, `stm32_iis3dwb_vibration_source.c`, `iis3dwb_units.h`, collecte FIFO, `live_metrology.c`, indicateurs RMS/crête, configuration active FRAM/B4, données B3 réellement reçues, API/SQLite, graphes UX-L2. Vérifier la FS effective sur capteur (codes 0/1/2/3 = ±2/4/8/16 **g**, sensibilités 61/122/244/488 µg/LSB) et les unités mg de bout en bout. **±2 g ≠ ±2 mg**. Identifier objectivement où apparaît 2 mg (domaine Y, amplitude de variation, limitation logicielle, conversion ou écrêtage physique). Mesures antérieures RMS global ~960–1100 mg : comprendre si gravité domine et ne jamais déduire une vibration RMS de cette valeur seule.
2. Distinguer **calibration du temps/cadence FIFO (s_calibrate)** de **calibration d'accélération/biais/gravity removal** ; inspecter le calcul exact actuel avant d'affirmer que la compensation de 1 g existe ou non. Envisager un modèle de compensation de composante DC **par axe sur une fenêtre définie**, pour fournir une grandeur de vibration sans confondre orientation, gravité, accélération réelle basse fréquence ou étalonnage du zéro ; expliquer la portée bande passante/DC, effets de montage et démarrage, et limites de mesure. Pas de soustraction aveugle de 1000 mg au RMS global, pas d'offset inventé, pas de constante hardcodée, pas de recalibration destructive ni de renormalisation silencieuse d'indicateurs normatifs gelés.
3. S'il manque une spécification pour changer la **sémantique normative RMS/crête**, **documenter le constat et une proposition séparée clairement étiquetée, avec validation structurante requise**, sans transformer les valeurs historiques ni modifier tacitement le mapping B3. Pour une correction logicielle objectivement démontrée (unités/échelle/affichage), corriger et tester immédiatement avec avant/après et contrôles réels. Ne modifier plage matérielle que sur saturation physique constatée et contrôles SNR/résolution, configuration autorisée et preuves. Préserver configuration et identité durables, aucun effacement FRAM ou reprovisionnement.
4. Fournir preuves comparant **brut XYZ → conversion mg → RMS/crête B3 → SQLite/API → courbe**, avec distinction mesure statique et vibratoire ; éventuellement essai capteur au repos / vibration contrôlée si matériel accessible et sans risque. N'inventer ni valeur vibratoire corrigée ni calibration effective. Le cas échéant demander un geste physique simple à l'utilisateur et seulement s'il est indispensable.

## P1-B — Rafraîchissement visuel véritable et opérationnel, avant démonstration

Refondre **la présentation**, pas l'autorité métier ni le modèle S7 :
- **Vue générale/parc** : une ligne par vrai TR2, identité, équipement/emplacement uniquement si réellement connus, état communication, acquisition RUNNING/STOP/HELD, fraîcheur, sévérité réelle B3, RMS global et crête globale côte à côte en mg, moment de dernière réception, alerte vs simple STOP distingués. Valeur ancienne affichée avec badge texte explicite « dernière connue » et âge ; jamais en direct par tromperie. Navigation/filtres/tris existants préservés.
- **Détail Synthèse et Vibrations** : hiérarchie au-dessus de la ligne de flottaison à 1080p : en premier KPIs avec unités, état et qualité, graphique RMS/crête central lisible, XYZ superposables avec contrôles discrets, date provenance réelle ; graphes responsive avec échelles adaptées et graduations honnêtes, pas de limitation imaginaire à2 mg. Montrer tendance et variation réellement disponibles, expliquer la part statique s'il n'y a pas de compensation ; ne pas rebaptiser le RMS absolu « vibratoire » sans preuve.
- **Autres onglets** État & config, Commandes, Campagnes, Diagnostic : style cohérent, espaces, typographie, regroupements, informations utiles d'abord, avertissements techniques et grandes tables de registres en sections repliables. Indiquer « indisponible dans LiveDiagnostic » pour B1/B2/B7/run/epoch/config. Préserver état et six onglets, responsive bureau, contraste, clavier, auto-refresh sans flicker ni refetch excessif. Éviter grand texte brut JSON et panneaux techniques avant mesures ; pas de fausse mesure, seuil, événement ou état commande.
- Revoir les pages `live-fleet.html`, `live-device.html`, feuilles CSS/JS S7 concernées, avec aperçu **vrai navigateur Edge** à 1920×1080 et largeur étroite raisonnable. Comparer captures avant/après et tester valeurs LIVE (fresh/HELD/lost), vrais lots SQLite, navigation/axes/CSV. Aucun serveur d'archive substitué à une preuve LIVE actuelle ; le réemploi de preuves anciennes est explicitement historique. Ne pas refaire les acquis sans raison.

## P2 — START/STOP Web sécurisé, si temps et validation permettent

Concevoir une intégration dans la **même composition Service** permettant une demande opérateur Web contrôlée par le moteur transactionnel éprouvé, sans deuxième maître COM4, ni l'envoi Modbus direct depuis le navigateur. Auth/port localhost et protection action volontaire à expliciter ; éviter endpoints POST non protégés, CSRF/local cross-origin et écriture involontaire. Réutiliser pause tokenisée avec preuve de handle libéré, exclusivité COM4, allocation epoch/tx persistée dans **ledger actuel**, lectures gardes B0/B3/B5, terminal+effet, aucune répétition auto, état demandé vs effectué/incertain (STOP ~40s), refus pendant transport perdu et profil non supporté. Ne pas dupliquer les deux autorités Web S7/profil partiel, décider un adaptateur unique si faisable sans refonte. `START.cmd`/`STOP.cmd` restent recours qualifié. En cas de risque/scope trop grand, préparer uniquement la conception documentée et tests logiciels, livrer P1 pleinement avant 20/10 et reporter implémentation sécurisée.

## Environnement physique et logiciel : gel utile avant la démo

Dernier rapport COM4 (preuve au 19/10 ~15:04 UTC) : STM32 `c36c971`/BIN SHA `9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981`, STOP5/21/HELD, FRAM SHA `97f43a53316248d705fbf86d9efe2ad8fa93ab2d37c2af937cfcc068e69455f3`, journal110/root123/epoch5/run15/config3gen3. **Service corrigé laissé actif** localhost5100/COM4, publication complète `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-COM4-HELD-20261019\` codecontenu d67741f2 et lanceurs anciens redirigés. Depuis, l'utilisateur a testé et indique « ça semble fonctionner » ; état **actuel matériel et service à recontrôler** sans présumer STOP et sans perturber son essai.
- Avant nouveau déploiement Windows : vérifier que la **publication complète** est acceptée sous SAC, SHA des artefacts, smoke COM99 sans matériel avec HTTP réel et aucune erreur Code Integrity. **Ne pas** mélanger DLL dans paquet accepté ; ne pas écraser/réinitialiser l'actuelle installation sans point de retour. Si nouveau service bloqué, garder démo physique actuelle et livrer solution visuelle compatible si légitimement possible, sans contournement de politique.
- Accès debug/Flash seulement si nécessaire, hors FIFO et avec procédure GDB corrigée **COM4_DIAGNOSTIC.md** : jamais injecter READ FRAM 262144 dans callback chronométré ni altérer budgets ; après chaque audit prouver B0/B3 réels post-détachement **sans reset**. `allow_flash:true`, `allow_debug:true` avec garde FRAM/SD/Option Bytes/identités ; conserver dernier firmware démontré sauf modification clairement justifiée, preuve readback et capacité STOP. Aucune ancienne clé B5 réutilisée, transactions5/12..21 consommées, pas de reset ou flash cosmétique.
- Ne pas nettoyer worktrees/stashes/main local historique au cours de cette mission ; la consolidation et signature durable restent tranches distinctes. Échecs documentés, aucun résultat maquilllé.

## Acceptance, rapport et contrôle

1. P1-A : diagnostic étayé du «2mg», FS réelle et calibration gravité, recommandations correctives fondées et absence de mutation normative silencieuse.
2. P1-B : amélioration visuelle réellement visible à l'écran, tous onglets avec priorité de supervision, preuve snapshots navigateur lisibles à 1080p, tests de fraîcheur/qualité/valeurs, URL LIVE et continuité service. Ne pas promettre une mise à jour déployée avant test Windows du paquet complet.
3. P2 : START/STOP Web seulement si transactionnel, sûr et testé physiquement ; sinon conception et dette claire, .cmd physiques maintenus.
4. Zéro altération de preuves existantes ou d'identité FRAM, Windows CI inchangé, refus explicites, no mock masquerading as physical. Critères de demo : matériel LIVE/COM4 vrais, opérateur simple, graphique lisible, état terminal STOP sûr.

Codex produit code/tests/docs/proofs proportionnés, push normal sur `main`, `DERNIER_RAPPORT.md` status DONE/BLOCKED honnête et preuves de relecture distante. ChatGPT fera contrôle indépendant après publication. En cas de décision métrologique structurante (soustraction DC normative) ou de sécurité Web ouverte, escalader une décision ciblée plutôt que risquer une fausse grandeur ou une commande non sûre.
