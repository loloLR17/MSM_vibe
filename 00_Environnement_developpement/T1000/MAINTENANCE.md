# T1000 — Sécurité et maintenance du développement

Authentifications : Codex natif ChatGPT, fichiers personnels hors Git avec mode 600 ; paramètres permanents conservés. GitHub CLI via coffre Windows/GCM ; hosts.yml ne contient plus oauth_token. Le pont n'injecte le jeton que dans le processus gh, et le helper Git le transmet par le protocole stdin/stdout, sans fichier ou argument secret. Le secret demeure accessible aux processus du même utilisateur : le coffre protège le stockage, pas une session utilisateur compromise. Aucun export global GH_TOKEN, aucune clé SSH nouvelle, aucune modification Git globale.

Git local : identité confirmée par l'utilisateur, origin HTTPS, branche main, helper limité à github.com et utilisateur loloLR17. Le pont échoue si le coffre est indisponible ; les builds Linux restent possibles hors ligne. Ne pas inclure `.codex`, `.config/gh`, données SQLite ou logs d'auth dans un commit. Pour une nouvelle connexion gh, utiliser le parcours d'import documenté dans REPRISE_QUOTIDIENNE.

Sauvegardes : clone pré-migration complet (inclut .git et correctif), patch exact et archive des anciens espaces hors Git, avec SHA256 et liste de membres. Copie des preuves et sauvegardes dans Documents. Une sauvegarde sur le même SSD ne remplace pas une sauvegarde externe : conserver une copie indépendante selon la politique de l'utilisateur. Aucune donnée personnelle nettoyée, aucune clé de récupération manipulée.

Maintenance du service binfmt : surveiller après update WSL ou ajout de binfmt.d. Le drop-in ciblé est documenté, conserve les erreurs réelles et évite de supprimer WSLInterop. Réexaminer/supprimer seulement après une correction stable et contrôle après redémarrage. Aucun montage, compte ou sudoers modifié.

Maintenance paquets : apt/dpkg et dépôts Debian/Microsoft officiels inspectés ; aucun upgrade global opportuniste. Les mises à jour de sécurité relèvent de l'entretien régulier. Node Windows/Linux et compilateurs ARM Windows/Linux servent deux environnements distincts ; leur coexistence est intentionnelle. Aucun CubeIDE nécessaire au cycle CLI qualifié.

Les sorties firmware et bin/obj .NET sont régénérables. Les preuves sous reports et backups ne le sont pas : ne pas les confondre avec un cache. Les installateurs officiels retenus ont des empreintes ; les extractions redondantes peuvent être régénérées. Ne pas compacter le VHD WSL à chaud ; aucune compaction ou partitionnement dans cette mission.

Les contrôles administrateur complémentaires et le relevé d'espace disque détaillé sont dans le dossier local de clôture. Instruction utilisateur : pas de nouvelle UAC, aucune action BitLocker/protecteur/clé, ces contrôles ne bloquent pas le développement TR2. Pare-feu et antivirus restent actifs. Aucun paramètre de sécurité n'est affaibli.

Pour chaque nouvelle mission : lire AGENTS et le journal versionné, relever Git/HEAD, définir les allow_* ; aucune autorisation de cette clôture ne se prolonge. Toute opération STM32 doit vérifier SD, cible, artefact et procédure. Ne pas lancer un firmware pouvant écrire sur une carte microSD non autorisée.

Publication GitHub : la protection GH007 a refusé les commits portant l'email privé initial. Adresse ID+noreply du compte vérifiée puis configurée uniquement localement ; nom conservé. Les commits non publiés de cette mission ont été recréés sous sauvegarde locale, sans perte de contenu, force push, rebase ou modification distante de sécurité. Ne pas republier la référence backup privée ni désactiver la protection d'email.
