# AC1-R2 — investigation Windows du 29 octobre 2026

Preuves strictement readonly de la révision du contrat publiée au778776be. Paquets R1/R2 existants inchangés ; aucun gate, lancement de paquet, redéploiement, changement de politique, restartService ou accès STM32/COM4 direct par Codex. Le service démontré continue ses lectures habituelles.

Chaque `.zlib` contient les octets exacts du fichier indiqué dans BUNDLE ; empreintes brutes et compressées. La collecte Windows retourne des champs structurés et retire SID/nom du PC ; chemins de fichiers des événements limités au basename. Certificats observés : informations publiques Microsoft uniquement, aucun secret ou clé privée exportés. Les commandes sont conservées pour reproductibilité.

La première requête temporelle a renvoyé0événement de façon incohérente avec le journal conservé. Contrôle de rétention874records, puis lecture bornée des2000derniersrecords et corrélation ActivityID :592événements pertinents dont296ID3089 dans148groupes. La première collecte locale est conservée mais n’est pas l’autorité de conclusion ; aucune absence de3089 ou perte de logs annoncée.

NuGet : les hashes `sha512_manifest` sont des hashes de contenu NuGet ; le SHA512 de l’archive signée complète est enregistré séparément et comparé au fichier`.nupkg.sha512`. Ne pas les confondre. Les trois packages effectivement refusés/sous-jacents ont passé `dotnet nuget verify --all`; chaque DLL publiée correspond exactement au membre de son archive. `Microsoft.Data.Sqlite` métapackage n’a pas subi cette commande ; la DLL runtime Microsoft a été examinée par Authenticode. Le booléen null dans la provenance signifie nonexécuté, paséchec.

Le rapport historiquee81df0e4 est conservé comme snapshot de preuve, pas remplacé ni réécrit. La nouvelle conclusion resteBLOCKED, liée à l’absence de moyen de signatureéditeur légitime disponible et non à un défaut RS485 ou à un résultat de testsfirmware.
