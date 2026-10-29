# AC1-R2 — confiance Windows, diagnostic du 29 octobre 2026

Mission `TR2-20261028-ACCEL-FS8G-VIBRATION-AC`, révision de reprise `778776be`. Cette note décrit les observations sur les paquets existants ; elle ne définit aucune exception de sécurité ni modification du protocole TR2.

Le blocage immédiat est prouvé : Windows refuse de charger deux DLL sans signature Authenticode dans R2. La raison exacte de la différence d’acceptation entre R1 et R2 reste indéterminée. Les sources et les fichiers des dépendances n’ont pas été remplacés par le diagnostic.

| Élément | Résultat observé |
|---|---|
| Manifestes complets R1/R2 | 378 fichiers chacun, inchangés depuis les publications ; 361 identiques entre paquets, 17 différents |
| `NModbus.Serial.dll` | Même SHA-256 `1714c12265bf336cff5fd776f97f922fea9b3f0291ccd06e5ccc6c23d41be1c0`, Authenticode `NotSigned` dans les deux |
| `SQLitePCLRaw.batteries_v2.dll` | Même SHA-256 `815ce410c9eab531dd219318e3ba16c5e38e83d361cfbbdbb4bbca222c497b0b`, Authenticode `NotSigned` dans les deux |
| Événements R2 du gate conservé | 148 groupes par ActivityID : 146 NModbus.Serial et 2 SQLitePCLRaw ; chacun avec 3033, 3077 et deux 3089 |
| Signature dans les 296 événements 3089 | `TotalSignatureCount=0`, publisher/issuer `Unknown`, dates sentinelles1601 ; pas une preuve de certificat révoqué/expiré |
| Refus 3077 | Statut `0xc0e90002`, niveau demandé2 / validé1, politique `0283ac0f-fff1-49ae-ada1-8a933130cad6` |
| Signature NuGet des archives | `dotnet nuget verify --all` réussit pour NModbus3.0.83, NModbus.Serial3.0.83 et SQLitePCLRaw.bundle_e_sqlite3 2.1.12 |
| Provenance des DLL | Correspondance intégrale SHA-256 avec les membres net6.0 et netstandard2.0 des archives NuGet signées par le dépôt nuget.org |
| Exemple de signature PE existante | `Microsoft.Data.Sqlite.dll` : Authenticode `Valid`, signataire Microsoft ; chaîne inspectée hors révocation, aucune clé privée disponible déduite de ce certificat public |
| Moyen de signature local | Aucun certificat codeSigning trouvé dans `CurrentUser\My` ou `LocalMachine\My` ; aucune chaîne de signature configurée dans les scripts/projets inspectés |

Une signature de dépôt NuGet protège le package ; elle ne signe pas individuellement le fichier PE extrait. Les [signatures NuGet](https://learn.microsoft.com/en-us/nuget/reference/signed-packages-reference) et l’acceptation de chargement par Windows sont deux vérifications distinctes.

Les [événements App Control](https://learn.microsoft.com/fr-fr/windows/security/application-security/application-control/app-control-for-business/operations/event-id-explanations) permettent de corréler les refus et les signatures par ActivityID. Ici l’absence de signature est observée, mais la source de réputation/caching/contexte ayant permis R1 n’est pas reconstituée. Aucune interprétation non documentée de `CacheState=3` ; `VerificationError=0` d’un 3089 sans signature ne vaut pas acceptation du fichier. L’absence de refus R1 n’explique pas à elle seule sa réussite.

La copie initiale était complète, vers deux chemins NTFS distincts, à des heures et avec des binaires parents différents. Les fichiers inspectés n’ont actuellement que le flux `:$DATA`, sans Zone.Identifier observé. Cela ne décrit ni l’état historique des métadonnées au premier chargement ni la décision cloud. Pas de test de déplacement/renommage ou de changement d’empreinte pour contourner le refus.

## Moyen nécessaire pour la suite

Fournir un **canal de signature éditeur approuvé**, avec certificat légitime utilisable ou profil distant d’Artifact Signing déjà autorisé et opérationnel. Le résultat attendu est une publication complète dont les composants concernés sont signés de façon vérifiable et horodatée, en préservant provenance/licences et versions originales. Les [instructions Microsoft pour SAC](https://learn.microsoft.com/en-us/windows/apps/develop/smart-app-control/code-signing-for-smart-app-control) décrivent ces voies ; aucun certificat n’est acheté/créé/importé par cette mission et aucune signature autoémise n’est proposée.

Si les éditeurs fournissent les mêmes dépendances en distribution Authenticode reconnue, cela peut constituer une autre voie, à vérifier sur ses propres octets et sa provenance. Aucun tel artefact n’est disponible dans les paquets/cache inspectés ; une mise à niveau arbitraire des dépendances n’est pas justifiée par les preuves actuelles. Une signature NuGet déjà valide ne suffit pas à débloquer les DLL constatées.

L’arbitrage minimal est donc **désigner/fournir le canal de signature conforme et son accès**, ou obtenir de l’éditeur une publication reconnue. Ne pas fournir de clé privée dans le rapport ou le dépôt. Certificat ou signature valide ne promettent pas le résultat du gate : il faudra contrôler le paquet complet, les quatreHTTP200, SAC actif et l’absence de refus corrélés, d’abord sur COM99. La [FAQ SAC](https://support.microsoft.com/fr-fr/windows/security/threat-malware-protection/smart-app-control-frequently-asked-questions) ne propose pas d’exception par application.

Aucun gate identique n’a été répété pendant cette investigation. Le service démontré est resté PID8312, en observation B3/STOP, ledger5/25 ; aucune commande de cible, flash, reset ou mutation FRAM/SD. La qualification physique AC1/±8g reste en attente et les essais DSP/ARM suivants ne sont pas lancés.

[Rapport et preuves](../../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md).
