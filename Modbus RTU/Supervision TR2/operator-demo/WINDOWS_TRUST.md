# Déploiement Windows — contrôle de confiance explicite

Mission `TR2-20261019-WINDOWS-TRUST-LIVE-RESUME`. Aucun affaiblissement de sécurité n'est nécessaire ni permis par cet outillage.

## Constats du19/10

Le GUID bloquant0283ac0f-fff1-49ae-ada1-8a933130cad6 est la politique Microsoft **VerifiedAndReputableDesktop / Smart App Control**, pas la preuve d'une règle d'entreprise configurable par ce projet. Lecture locale : état registre1, événement3099 activant cette politique avec statut0. La session n'est ni administratrice ni SYSTEM ; CiTool -lp -json retourne0x80070005. Les magasins CurrentUser/My et LocalMachine/My ont été lus avec succès et n'exposent aucun certificat utilisable via CodeSigningCert. Aucun profil Azure CLI ou module de signature cloud détecté dans l'environnement interrogé. Cela n'exclut pas l'existence d'un service de signature externe, mais aucun n'est configuré ici.

Les deux Domain.dll comparées sont non signées, même taille19968octets mais empreintes différentes ; pas de Zone.Identifier constaté. Ancienne R5-B2 : propriétaire Administrateurs et EA `$KERNEL.PURGE.ESBCACHE` ; candidate du checkpoint : propriétaire utilisateur, pas d'EA avant nouveau lancement. **Aucune EA copiée, effacée ou interprétée comme autorisation.** Cette EA n'est pas la preuve d'un origin claim Managed Installer. Les journaux disponibles ne prouvent pas le motif précis d'acceptation ancien ; aucune hypothèse de répertoire de confiance ou de politique désactivée retenue.

L'exécutable R5-B2 redémarre réellement aujourd'hui sur COM99 inexistant, retour0. Sa route overview récente n'existe pas (HTTP404 attendu pour cette ancienne version) ; ceci ne valide pas UX-L2. La publication du checkpoint `c36c971` démarre maintenant elle aussi **à octets inchangés** : vérification des377 fichiers applicatifs, HTTP200 sur V1/overview/history/fiche, sortie0, aucun refus3033/3077 corrélé et Smart App Control toujours1. Le premier collecteur HTTP a été lancé trop tôt ; un contrôle complet a ensuite prouvé les réponses. Une erreur de décodage OEM CP850 du collecteur a été corrigée hors ligne en conservant les octets bruts.

Cependant la recompilation après correction ciblée de pause produit une nouvelle Service.dll SHA-256 `21a440e3ec32f1e974e6c89765749852c3d83e1d9d6d6b767ada53acb67a2d17`, refusée à son tour (FileLoadException0x800711C7, événements3033/3077, même politique). Copie versionnée distincte `TR2-LIVE-DEMO-20261019-resume`, pas de remplacement de fichiers dans un ancien répertoire accepté. **Une autorisation constatée pour une version ne s'étend pas aux nouvelles empreintes.** La réputation ISG/cache constitue une explication possible, pas une décision précisément auditée dans ces traces. La méthode durable de signature d'éditeur n'est pas disponible localement.

## Procédure réutilisable sans auto-allow

Le script WSL Python standard `tools/verify_windows_deployment.py` lance le **vrai exécutable Windows**, sur COM99 inexistant uniquement, et conserve commande/horodatages, empreintes applicatives avant/après, console, HTTP et événements CI corrélés. Il exige sortie0, quatre réponses HTTP200, zéro blocage corrélé, Smart App Control1 et octets inchangés. Un résultat négatif interdit de déclarer ce paquet accepté. Aucun flash, port COM4, modification de registre/politique, ADS, certificat ou installation de runtime. Les preuves doivent aller dans un répertoire neuf.

```bash
python3 tools/verify_windows_deployment.py \
  --application /mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019/app \
  --evidence /home/lolo/dev/msm/reports/NOUVEAU_REPERTOIRE_DE_CONTROLE
```

Répéter ce **contrôle de déploiement** sur une nouvelle publication est nécessaire ; cela ne justifie pas de refaire les tests firmware ou campagnes déjà acquis. Le contrôle ne crée aucune autorisation, ne soumet aucun fichier en ligne et ne garantit aucune acceptation future. Ne pas déplacer/renommer/remplacer des DLL dans une publication acceptée pour éluder un refus. Le répertoire de version sert à la traçabilité, pas à obtenir la confiance.

## Autorité requise pour les prochaines publications

Microsoft documente la réputation de fichier et la signature issue d'un fournisseur de confiance ; Smart App Control n'offre pas d'exception manuelle par application. Aucun bouton « autoriser cette DLL » ou commande locale d'exemption n'est donc promis ici. La voie stable est une **chaîne de signature d'éditeur approuvée** (certificat CA avec clé utilisable autorisée, ou profil Artifact Signing/canal de publication officiellement approuvé). La signature devra couvrir les binaires nécessaires, être vérifiée, puis passer le contrôle d'exécution Windows ; une signature seule n'est pas un auto-allow universel. Aucun certificat auto-généré, clé empruntée ou règle ajoutée hors autorité.

Le propriétaire de ce canal doit désigner le certificat/profil, les droits d'usage et l'identité de l'éditeur. Codex peut ensuite automatiser compilation/copie/signature/vérification et contrôle de lancement, sans déléguer la manipulation des DLL à l'utilisateur. Ne pas communiquer de clé privée ou de secret dans le dépôt. À défaut, une qualification physique limitée à la publication c36c971 actuellement acceptée exige une décision explicite sur la portée et la réserve de fiabilité de l'ancien acquittement ; elle ne résout pas la confiance des prochaines recompilations.

## Sources Microsoft consultées

- [Politiques fournies avec Windows, GUID et événements3099](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/operations/inbox-appcontrol-policies).
- [Fonctionnement Smart App Control](https://learn.microsoft.com/en-us/windows/apps/develop/smart-app-control/overview).
- [FAQ — absence d'exception individuelle et signature](https://support.microsoft.com/en-us/windows/security/threat-malware-protection/smart-app-control-frequently-asked-questions).
- [Signature pour Smart App Control](https://learn.microsoft.com/en-us/windows/apps/develop/smart-app-control/code-signing-for-smart-app-control).
- [Événements CI et corrélation des décisions](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/operations/event-id-explanations).
- [ISG et attributs d'origine](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/design/use-appcontrol-with-intelligent-security-graph).
- [Artifact Signing, service Microsoft](https://learn.microsoft.com/en-us/azure/artifact-signing/overview).

## Complément COM4 LOSS DIAG —19/10/2026

Une nouvellepublication complète corrigeant HELD et intégrant la pause stable a effectivement passé le contrôleWindows, à son propre emplacement TR2-LIVE-COM4-HELD-20261019/app :377fichiers exacts, Service.dll63f3ddc1…cb85fa, quatreHTTP200/exit0/SAC1/zéroCI corrélé. Le premier chargement a dépassé le délaiHTTP25s (~35s) mais le service s’est exécuté ; échec conservé. Le contrôleur autorise désormais une attente explicite60s (1..120), sans modifier les critères de succès ou la politique. La confirmation et les essaisCOM4 précèdent toute redirection des lanceurs. Raison interne de l’acceptation non établie ; ancienneDLL21a440 encore distincte, aucune confiance universelle des futurescompilations. Aucuneprotection/ADS/EA/certificat/règle modifiée ; signaturedurable toujours séparée.
