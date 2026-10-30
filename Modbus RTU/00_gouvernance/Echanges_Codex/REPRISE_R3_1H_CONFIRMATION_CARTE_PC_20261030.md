# MSM TR2 — Signal de reprise : microSD disponible sur le T1000

**30 octobre 2026 — Déclaration de l'utilisateur transmise par ChatGPT — mission en cours `TR2-20261029-R3-1H-FAT32-ENDURANCE`.**

> « Effectivement je ne suis pas en permanence devant mon poste T1000. La carte SD est insérée dans le T1000 prête à l'emploi. »

## Portée exacte de la confirmation

L'utilisateur confirme que **la microSD a été physiquement transférée dans le lecteur du T1000 et est prête pour la récupération**. Le transfert physique qui bloquait l'étape E4 est donc **déclaré effectué**. **L'utilisateur ne confirme pas explicitement dans ce message que TR2-002 est hors tension** ; son état électrique actuel reste **inconnu**, de même que la lettre du lecteur et l'identité du volume jusqu'à relecture réelle sur le PC. Ne pas fabriquer ces confirmations.

Cette note **n'est ni un nouveau contrat, ni un rapport de tests, ni un statut `DONE`, ni un ordre de relancer une acquisition**. Elle ne modifie ni les droits explicites de `MISSION_EN_COURS.md`, ni le rapport `DERNIER_RAPPORT.md` possédé par Codex. Dernier état distant contrôlé lors de la rédaction : branche `main` au SHA `77cc1e7c91e201656714fc7712f0e6879a8366b8` ; contrat `READY` de l'identifiant ci-dessus, dernier rapport `BLOCKED` au même identifiant.

## Reprise opérateur déjà autorisée, sans nouveau geste humain a priori

Codex, **lorsque sa session sera effectivement relancée ou reprise**, doit reprendre **la même mission**, **à E4 uniquement**, à partir du rapport et de `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/REPRISE.md`, sans reflash, nouveau START, mise sous tension, reset, nouveau formatage ou nouvelle capture de l'heure. La mission est déjà encadrée par `AGENTS.md` et la gouvernance existante ; ne pas demander d'approbation logicielle répétitive.

1. Sur le **T1000**, vérifier réellement la présence du lecteur amovible, la compatibilité du volume FAT32, le fichier `TR2INFO.TXT`, la capacité et l'identité déjà documentées ; déterminer la lettre actuelle au lieu de supposer `F:`. Si l'identité ne concorde pas, arrêter les opérations sans écrire.
2. Vérifier en lecture seule que `H1N0003.TR2` et `H1N0005.TR2` sont présents, sans écraser les fichiers historiques. Puis utiliser `/home/lolo/dev/msm/reports/TR2-r3-1h-fat32-endurance/extract_hour_pc.py` en respectant ses préconditions vérifiées, pour **copier vers le PC sans écraser** et valider indépendamment l'intégralité des records, CRC, indices, temps, session, footer, compte et SHA256 de tous les **57 088 872 octets utiles attendus**, face à l'empreinte producteur amont `e99b50584536231ff881d15637fb5fd0e6b0170908fb2a3c04f6e457338c7ce1`. Valider aussi le gate `H1N0003.TR2`, ses **31 554 octets** et SHA amont `dc2a19be588e2d7d6006e7fd88e3c7824931f7adbcb95148a58551c1b4886a88`. Vérifier les fichiers préexistants.
3. Préserver le problème de preuve temporelle : le STM32 rapporte **3 600,000743 s nominales** mais **aucune référence de durée physique monotone externe qualifiée** ne vient confirmer l'heure réelle. La comparaison hôte/WSL publiée est perturbée et ne doit pas être assimilée à un chronomètre indépendant. **Ne pas transformer un SHA PC conforme en qualification horaire stricte** ; rapporter distinctement le résultat d'intégrité et la réserve métrologique temporelle.
4. Publier les preuves E4 sur GitHub, le statut `DONE` uniquement si les critères complets sont honnêtement satisfaits, sinon `BLOCKED` avec checkpoint et plan minimal sans répéter silencieusement l'heure ; effectuer la relecture distante. ChatGPT se chargera de la revue A-01 indépendante après le nouveau rapport.
5. Aucune dépendance à la disponibilité physique immédiate de l'utilisateur pour **la seule extraction PC en lecture/copie**. Si un nouveau geste physique est réellement nécessaire, le demander une fois, concrètement. Ne pas présumer TR2-002 hors tension et ne pas agir sur le STM32 pendant cette reprise.

**Principe inchangé pour R3-8H :** campagne embarquée autonome après déclenchement et déconnexion complète de Codex/GDB, sans quota Codex consommé pendant huit heures. La future mission sera distincte seulement après clôture et qualification de la mission 1h.
