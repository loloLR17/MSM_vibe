# AGENTS.md — Projet MSM / Capteur de vibration TR2

## 1. Objet et contrat de mission

Ce repository contient le projet MSM — Capteur de vibration TR2.

Codex intervient comme agent opérateur local pour les tâches logicielles et d'outillage.

Le travail est effectué en **mode rigueur stricte**.

AGENTS.md définit les règles de gouvernance **V2-B2 candidate**, applicables uniquement dans la branche expérimentale tant que cette version n'est pas intégrée à `main` après revue humaine. Le contrat de mission définit le travail particulier à effectuer et peut préciser :

- OBJECTIF ;
- PÉRIMÈTRE ;
- CRITÈRES D'ACCEPTATION ;
- VALIDATION REQUISE ;
- AUTONOMIE ACCORDÉE ;
- CONDITIONS D'ARRÊT / ESCALADE ;
- SORTIE ATTENDUE ;
- `target_branch` : branche autorisée ;
- `allow_commit`, `allow_push`, `allow_flash`, `allow_debug` : permissions explicites, toutes à `false` par défaut.

Lorsque l'objectif, le périmètre et les critères d'acceptation sont clairement définis, Codex agit de façon autonome dans ce cadre. Les restrictions explicites de la mission prévalent sur les permissions générales de ce fichier. L'utilisateur reste responsable des interventions physiques.

Les règles de ce fichier s'appliquent à l'ensemble du repository sauf instruction plus locale et explicitement documentée.

---

## 2. Source de vérité

La seule référence documentaire et logicielle est l'état réel courant du repository Git.

Ne jamais travailler à partir :

- d'un ancien ZIP ;
- d'une ancienne copie du repository ;
- d'une version mémorisée du projet ;
- d'une ancienne conversation ;
- d'une hypothèse sur l'état du code.

Avant toute modification, vérifier au minimum `git status`, `git rev-parse HEAD`, la branche courante et les références distantes pertinentes. Sur `main`, effectuer `git pull --ff-only origin main` **uniquement si l'état local permet cette opération sans risque pour les travaux préexistants**. Sur une branche de mission, récupérer et vérifier la référence de cette branche sans fusionner automatiquement `main` ; privilégier un clone ou worktree isolé si le dépôt principal est sale.

Ne pas résoudre automatiquement une divergence par merge, rebase, reset ou autre opération modifiant l'historique. Si l'accès réseau échoue, distinguer l'état local de l'état distant non vérifié ; ne pas supposer le dépôt à jour. Signaler tout blocage non résoluble dans le périmètre.

L'état réel du repository prévaut toujours sur toute description antérieure du projet.

---

## 3. Inspection obligatoire avant modification

Avant d'ajouter ou de modifier du code :

1. inspecter les fichiers existants concernés ;
2. rechercher les symboles, fonctions, structures et interfaces déjà présents ;
3. rechercher les tests associés ;
4. rechercher la documentation ou les décisions d'architecture associées ;
5. comprendre les conventions utilisées dans la zone concernée.

Ne jamais inventer :

- un fichier ;
- un chemin ;
- une fonction ;
- un symbole ;
- une structure ;
- une API ;
- une option de commande ;
- une convention ;
- un comportement matériel.

Ne jamais supposer qu'un élément existe sans l'avoir vérifié.

Toute hypothèse nécessaire doit être explicitement signalée.

---

## 4. Politique de modification

Effectuer la modification minimale permettant de répondre à l'objectif demandé.

Préserver autant que possible :

- l'architecture existante ;
- les conventions de nommage ;
- les interfaces existantes ;
- la structure des fichiers ;
- la séparation des responsabilités ;
- les mécanismes de validation existants.

Ne pas effectuer de refactoring opportuniste sans rapport direct avec la mission.

Ne pas corriger silencieusement un autre problème découvert pendant la mission.

Un problème annexe peut être signalé et documenté, mais il ne doit être corrigé que s'il est nécessaire à la mission ou si l'utilisateur l'autorise explicitement.

---

## 5. Tests et erreurs

Il est interdit de faire passer une validation en :

- supprimant un test ;
- désactivant un test ;
- affaiblissant un test ;
- modifiant artificiellement son résultat attendu ;
- contournant le code réellement testé ;
- masquant une erreur.

Une erreur de compilation ou de test doit être analysée à partir :

- du code réel ;
- de la sortie réelle de l'outil ;
- de la configuration réellement utilisée.

Ne jamais improviser une correction à partir d'une API ou d'une structure supposée.

Un échec de compilation, de test ou de validation ne provoque pas automatiquement un retour vers l'utilisateur. Si la cause peut être diagnostiquée à partir des éléments disponibles et si la correction reste dans le périmètre, Codex doit analyser l'échec, corriger, relancer la validation appropriée et répéter si nécessaire jusqu'à satisfaction des critères.

Solliciter l'utilisateur seulement lorsqu'une condition d'escalade de la section 8 est rencontrée. Ne pas contourner un problème simplement pour terminer la mission.

---

## 6. Validation proportionnée

Le script principal de validation est :

`tr2_validate.sh`

### Modification limitée au harness STM32

Lorsque la modification est strictement limitée au harness matériel STM32 et qu'aucune régression du cœur fonctionnel n'est raisonnablement plausible, utiliser :

    STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 \
    ./tr2_validate.sh --cross-build-only

Ne pas lancer systématiquement l'ensemble des tests hôte lorsqu'ils ne sont pas concernés.

### Modification fonctionnelle ou risque de régression

Si le cœur fonctionnel est modifié, si une interface partagée est touchée ou si une régression est raisonnablement plausible, utiliser la validation complète :

    STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 \
    ./tr2_validate.sh

En cas de doute entre les deux niveaux de validation, privilégier la validation complète ; demander à l'utilisateur si une condition d'escalade est rencontrée.

Ne jamais déclarer une validation réussie sans disposer de la sortie effective de la commande correspondante.

Une modification purement documentaire ou de gouvernance ne nécessite pas automatiquement une compilation ou des tests lorsque ceux-ci ne peuvent raisonnablement apporter aucune preuve supplémentaire. Inspecter alors le contenu et le diff pour vérifier les critères de la mission.

Justifier la validation choisie dans le rapport final.

Une compilation réussie ne constitue pas une validation physique.

---

## 7. Git

Avant modification, examiner l'état Git.

Ne jamais écraser ou supprimer les modifications ou fichiers non suivis appartenant à l'utilisateur.

Après modification :

    git status
    git diff

Examiner le diff avant toute proposition de commit.

Les opérations Git destructives ou susceptibles de perdre du travail sont interdites sans autorisation explicite.

Cela comprend notamment :

    git reset --hard
    git clean
    git checkout -- <fichier>
    git restore <fichier>

lorsque ces commandes risquent de supprimer du travail existant.

Ne pas effectuer automatiquement de merge ou de rebase pour résoudre une divergence.

### Commit

Dans une mission autorisant explicitement la modification du repository, Codex peut créer de façon autonome un commit local, sauf restriction du contrat, lorsque :

- l'objectif et les critères d'acceptation sont satisfaits ;
- la validation appropriée a réussi, lorsqu'elle est nécessaire ;
- le diff et l'état Git ont été inspectés ;
- aucun fichier hors périmètre n'est inclus ;
- aucun fichier ou travail préexistant de l'utilisateur n'est inclus par accident.

Vérifier le contenu indexé avant commit. Le commit doit être atomique et son message doit décrire clairement la tranche réalisée.

Si les critères ne sont pas satisfaits, ne pas créer un commit présenté comme une tranche validée.

### Push

Par défaut, aucun `git push` n'est autorisé. Une **mission explicitement validée par l'utilisateur** peut accorder `allow_push: true` et `target_branch: <branche>` : Codex peut alors effectuer des push normaux répétés de **ses propres commits dans le périmètre de la mission**, sans redemander une confirmation pour chaque push. `allow_commit: true` ne vaut jamais `allow_push: true`.

Avant chaque push : vérifier branche et destination, absence de fichiers hors périmètre ou de secrets, validations applicables, diff indexé et état distant ; utiliser une référence de destination explicite. Après push : contrôler le SHA distant et relire le livrable lorsque la mission le prévoit. Si la branche distante a divergé, arrêter et escalader ; pas de force push, de merge ou de rebase automatique. Un push sur `main` exige une autorisation de mission **mentionnant explicitement `target_branch: main`**, distincte d'une autorisation générique de push. Les restrictions de la plateforme ou du sandbox restent applicables.

---

## 8. Autonomie logicielle et escalade

Dans une mission clairement définie, Codex peut, sans confirmation à chaque étape :

- inspecter le repository et son historique ;
- rechercher dans les sources et la documentation ;
- modifier les fichiers appartenant au périmètre autorisé ;
- compiler et lancer les tests appropriés ;
- effectuer le cross-build STM32 ;
- analyser les erreurs et effectuer les corrections nécessaires dans le périmètre ;
- répéter les cycles modification/build/test jusqu'à satisfaction des critères ;
- examiner les artefacts, `git diff` et `git status` ;
- réaliser les commits, push et publications de rapports **uniquement** lorsque les permissions du contrat les autorisent, avec contrôle distant après publication.

Ne pas solliciter l'utilisateur pour une incertitude qui peut raisonnablement être levée par inspection du repository, de la documentation, de Git, des sorties d'outils ou par un test non destructif.

Codex doit s'arrêter et rendre compte lorsqu'au moins une des situations suivantes apparaît :

- une décision d'architecture non prévue par la mission est nécessaire ;
- les faits observés contredisent la spécification ou les critères d'acceptation ;
- une modification hors périmètre devient nécessaire ;
- les preuves disponibles sont insuffisantes pour conclure après les vérifications possibles dans le périmètre ;
- une opération potentiellement destructive non autorisée est nécessaire ;
- une ambiguïté ne peut pas être levée par inspection ou test non destructif.

Présenter les faits, le blocage et la décision ou autorisation attendue. Ne pas contourner le problème pour terminer la mission.

---

## 9. Interventions physiques et opérations sensibles

L'utilisateur reste responsable des interventions nécessitant réellement une action physique, notamment :

- coupure ou remise sous tension physique ;
- insertion ou retrait de microSD ;
- modification du câblage ou déplacement de jumper ;
- connexion ou déconnexion RS-485 ou d'un équipement ;
- modification du breadboard, du montage, du banc ou de la carte ;
- toute observation nécessitant une action physique non automatisée.

Codex doit indiquer précisément l'action attendue et attendre le retour de l'utilisateur.

Les opérations suivantes restent soumises à confirmation humaine explicite :

- mass erase ou effacement hors procédure de flash standard qualifiée ;
- modification des Option Bytes ou des protections ;
- changement de configuration de boot persistante ;
- écriture arbitraire non prévue en mémoire non volatile ;
- opération matérielle inhabituelle ou non couverte par une procédure qualifiée.

Codex peut préparer la commande, expliquer l'opération et analyser son résultat, mais ne doit pas exécuter ces opérations sans autorisation explicite.

---

## 10. Portée des autorisations matérielles

Une autorisation ponctuelle d'opération physique ou potentiellement destructive ne constitue pas une autorisation permanente et ne couvre pas automatiquement l'opération suivante.

Une mission peut toutefois accorder séparément `allow_flash: true` et `allow_debug: true` pour des cycles répétés de flash standard et de debug lorsque les chaînes correspondantes ont été explicitement qualifiées et documentées dans le projet et que les conditions des sections 11 et 12 sont satisfaites. Ces champs sont `false` par défaut. Cette autorisation reste limitée au périmètre et à la durée de la mission ; elle ne couvre pas les opérations sensibles de la section 9.

Une autorisation de debug n'autorise pas à elle seule un flash. En cas d'ambiguïté non résoluble par inspection, demander confirmation.

---

## 11. Flash STM32 standard

Tant que la chaîne de flash du projet n'a pas été explicitement qualifiée et documentée dans le projet, chaque flash reste soumis à confirmation humaine. Ne pas déduire une qualification d'une commande disponible ou d'un flash réussi.

Après qualification explicite et documentée de la chaîne (cible et ST-LINK identifiés, commande et adresse de programmation approuvées, artefact et critères de contrôle consignés), une mission avec `allow_flash: true` peut autoriser le flash standard autonome. Avant chaque exécution, Codex doit vérifier :

1. que la cible attendue est identifiée ;
2. que le programmateur/ST-LINK attendu est identifié ;
3. que l'artefact provient du build attendu ;
4. que l'adresse et la procédure de programmation correspondent à la procédure qualifiée du projet ;
5. que la commande réellement disponible correspond à cette procédure ;
6. qu'aucune opération supplémentaire destructive ou permanente n'est demandée.

Si ces conditions et l'autorisation de mission sont réunies, Codex peut répéter les cycles build/flash/debug/rebuild/reflash sans confirmation à chaque flash, sous réserve de la qualification et de l'autorisation du debug. Sinon, s'arrêter et demander l'autorisation ou la clarification nécessaire.

Les opérations sensibles de la section 9 ne sont jamais autorisées implicitement par un flash standard.

Une réussite de STM32CubeProgrammer prouve uniquement ce que sa sortie permet d'établir. Elle ne prouve pas à elle seule le bon fonctionnement fonctionnel ou physique du firmware.

---

## 12. GDB et diagnostic

Codex peut préparer une session GDB, lire les symboles et proposer des breakpoints ou watchpoints.

Après qualification explicite et documentée de la chaîne de debug, une mission avec `allow_debug: true` peut autoriser une session GDB interactive persistante et les opérations suivantes de façon autonome :

- halt ;
- reset logiciel ;
- breakpoints et watchpoints ;
- lecture de mémoire et de variables ;
- inspection de l'état du MCU ;
- répétition des cycles de diagnostic nécessaires.

Ces actions peuvent être répétées dans le périmètre de la mission sans confirmation à chaque occurrence. Tant que la chaîne n'est pas qualifiée, l'utilisation sur cible nécessite une autorisation humaine explicite.

Une opération GDB entraînant une reprogrammation relève des règles de flash ; un effacement ou une modification persistante inhabituelle relève des règles d'opérations sensibles. Une action hors périmètre ou susceptible de perturber une qualification physique non couverte par la mission nécessite une escalade et une confirmation explicite.

Les résultats GDB doivent être rapportés comme des observations. Ne pas transformer une observation en conclusion physique non démontrée.

---

## 13. Validation physique

Distinguer systématiquement :

1. compilation ;
2. tests hôte ;
3. cross-build STM32 ;
4. flash ;
5. exécution sur cible ;
6. observation via debugger ;
7. validation physique.

Ces niveaux ne sont pas équivalents.

Ne jamais annoncer qu'une fonctionnalité est physiquement validée sur la seule base :

- d'une compilation réussie ;
- de tests hôte réussis ;
- d'un cross-build réussi ;
- d'un flash réussi ;
- d'une inspection statique du code.

Une validation physique doit reposer sur les observations réellement effectuées sur le matériel. Une tranche nécessitant une preuve physique ne peut être déclarée physiquement validée que si les critères correspondants ont réellement été observés.

---

## 14. Preuves, conclusions et rapport final

Toute conclusion technique doit être proportionnée aux preuves disponibles.

Distinguer explicitement :

- fait observé ;
- résultat de test ;
- résultat de compilation ;
- lecture du code ;
- hypothèse ;
- déduction ;
- élément restant à démontrer.

Ne jamais déclarer une étape « validée », « terminée » ou « gelée » si les preuves nécessaires ne sont pas disponibles.

Si les informations disponibles sont insuffisantes, le dire explicitement et appliquer les conditions d'escalade.

À la fin d'une mission significative, le rapport final doit préciser, selon ce qui est pertinent :

- le HEAD initial ;
- les modifications réalisées ;
- les validations exécutées, leur justification et leurs résultats ;
- les erreurs rencontrées et les corrections importantes ;
- les hypothèses restantes ;
- les preuves obtenues et les éléments restant à démontrer ;
- l'état Git final ;
- le hash et le message du commit créé, le cas échéant ;
- l'identifiant de mission, la branche cible, le SHA du code réellement évalué et les permissions exercées ;
- le résultat du push et sa preuve de relecture distante, le cas échéant ; distinguer rapport publié et qualification technique.

---

## 15. État courant du projet

Ne pas inscrire dans ce fichier :

- un HEAD Git supposé courant ;
- un nombre supposé courant de tests ;
- un résultat temporaire de qualification ;
- un état temporaire de H3h-E4 ou d'une autre tranche.

Ces informations évoluent. Les règles temporaires et l'état courant d'une tranche appartiennent au contrat ou au rapport de mission, pas à AGENTS.md.

Les déterminer à chaque mission à partir :

- de Git ;
- du code ;
- des documents présents dans le repository ;
- des résultats réellement obtenus pendant la session.

---

## 16. Protocole d'échange et configuration d'exécution

Pour les missions ChatGPT ↔ Codex, suivre `Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md` pour l'identité de mission, la propriété des fichiers, les archives et la relecture distante. Ce protocole ne peut étendre les permissions définies par le présent fichier et le contrat de mission.

Les paramètres `sandbox_mode` et `approval_policy` sont distincts des autorisations de mission. Aucun changement permanent de `~/.codex/config.toml`, des règles d'approbation ou du niveau de sandbox n'est permis sans décision explicite de l'utilisateur. Un échec de permission ne doit jamais être contourné en élargissant silencieusement les accès. Les essais V2-B1 ne qualifient ni une configuration permanente ni les opérations matérielles.

---

## 17. Priorité générale

En cas de conflit entre vitesse et traçabilité, privilégier la traçabilité.

En cas de conflit entre une supposition et une vérification possible, vérifier.

En cas de doute sur une opération potentiellement destructive ou physique, vérifier le périmètre autorisé et la procédure qualifiée ; demander confirmation si le doute ne peut pas être levé.

En cas d'échec, analyser les faits avant de modifier le code.

Le but n'est pas seulement d'obtenir un résultat fonctionnel, mais de pouvoir démontrer pourquoi ce résultat est considéré comme valide.
