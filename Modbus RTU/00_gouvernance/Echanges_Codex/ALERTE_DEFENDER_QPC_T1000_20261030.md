# MSM TR2 — Alerte Microsoft Defender lors d'un témoin QPC, T1000, 30/10/2026

**Signalement de l'utilisateur à ChatGPT ; revue de sécurité ciblée à réaliser par Codex dans la mission active `TR2-20261029-R3-1H-FAT32-ENDURANCE`.**

## Faits observables, sans conclure à tort

Capture de **Sécurité Windows** fournie par l'utilisateur : entrée **« Menace bloquée »** le **30/10/2026 à 11:47** (horaire affiché Windows), sévérité **Grave**, détection **`Trojan:Win32/Injuke.C!MTB`**, état **« Supprimé »**. Élément affecté de type **`CmdLine:`** et lancement de `C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -NonInteractive -Command ...`.

La portion visible de la commande contient `[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)`, `[System.Management.Automation.Language.Parser]::ParseInput(...)`, un paramétrage `[switch]$Witness`, `[string]$Port`, `[int]$MinimumSeconds=3601`, `[int]$ExpectedRun=5`, et `[Diagnostics.Stopwatch]::Frequency`/`IsHighResolution` ; c'est **compatible avec un contrôleur de durée indépendante demandé pour R3-1H**, mais **le contenu de la commande n'est pas intégralement visible dans la capture**. Aucun résultat d'analyse de malveillance ni de faux positif n'est établi.

**Comparaison directe du dépôt** au HEAD lu `c565c4cd03720717ab83000fd5887a071fe23c63` : le commit `d379e701c59c91c14f281aa92e06f115f5daf516` prépare `experiments/r3_time_reference/tools/measure_reference.ps1`, une sonde QPC textuelle lisible fondée sur `Stopwatch`, `Get-CimInstance` et `SerialPort`; **cette version publiée ne contient pas `$Witness`, `MinimumSeconds` ou `ExpectedRun`**, donc **elle n'identifie pas à elle seule la commande réellement détectée**. Il peut s'agir d'une variante locale non publiée, d'un script dynamique construit pendant la mission, ou d'une exécution autre : vérifier au lieu de présumer.

## Traitement immédiat et autonome par Codex

**Suspendre toute nouvelle exécution de la commande PowerShell incriminée jusqu'à examen de sa provenance et de son texte intégral.** Ne pas restaurer l'élément Defender, ne pas créer d'exclusion, ne pas désactiver Defender, Smart App Control, ASR, journalisation ou protections du T1000, ni contourner ou échapper à la détection par simple changement de syntaxe/encodage.

Dans la **même mission**, sans changer `MISSION_EN_COURS.md` ni écraser `DERNIER_RAPPORT.md` sans constat réel :

1. Préserver les preuves **non exécutables** de l'alerte : nom exact de détection, horaire, chemin/commande complète et PID/parent si exposés, journaux `Microsoft-Windows-Windows Defender/Operational` correspondants, méthode de génération exacte du `-Command`, version de Defender. Ne pas copier de secrets dans le dépôt. Toutes collectes par commandes **read-only**, sans tentative de restauration/allow.
2. **Comparer littéralement** le texte complet de la commande signalée aux scripts locaux Codex et à la version GitHub ; identifier sa création et son auteur opérationnel (fichier, helper, commande générée, hachage si disponible). Auditer les instructions sensibles : `Invoke-Expression`, téléchargement, réseau externe, registre, injection de processus, `Add-Type`, élévation, persistance, écriture de fichiers, credential access, modification de protection. Ne pas conclure que `Parser::ParseInput` exécute forcément le texte qu'il analyse.
3. Si la commande correspond intégralement à un témoin de temps **explicable, déterministe, sans action risquée** et aucune autre alerte/IOC n'apparaît, documenter un **faux positif probable mais non confirmé**, sans recontester ou annuler la protection. Considérer la voie officielle de signalement Microsoft d'un faux positif, et préférer une méthode simple, auditée, **non bloquée par Defender** pour la qualification temporelle.
4. Si l'origine demeure inconnue, si le texte est différent des sources contrôlées, ou si d'autres indices de compromission existent : **stopper le volet Windows** et publier un checkpoint de sécurité `BLOCKED` ; ne pas continuer les essais sur une base potentiellement compromise. Préserver le firmware STM32 et les copies SD, ne pas multiplier les runs.
5. Reporter les constats réels à ChatGPT avec lien vers les preuves, distinction **code publié** / **commande réellement lancée** / **analyse de l'antivirus** / **état sécurité résiduel**, avant toute nouvelle qualification temporelle.

**Décision de pilotage :** l'alerte n'est pas automatiquement la preuve que Codex a infecté Windows, et la présence de `Stopwatch` n'est pas automatiquement une preuve d'innocuité. Les protections restent actives. Cette note de gouvernance/incident **n'est pas une nouvelle mission ni une autorisation d'outrepasser Defender** ; aucun test n'est déclaré réalisé par ChatGPT sur le T1000.
